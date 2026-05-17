#pragma once

#include <algorithm>
#include <exception>
#include <ranges>

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/modules/export.hpp"
#include "wincpp/modules/module.hpp"
#include "wincpp/modules/object.hpp"
#include "wincpp/modules/section.hpp"
#include "wincpp/patterns/pattern.hpp"
#include "wincpp/process.hpp"

namespace wincpp::modules
{
    namespace detail
    {
        inline const IMAGE_NT_HEADERS* nt_headers( const std::shared_ptr< std::uint8_t[] >& buffer, const std::uintptr_t offset ) noexcept
        {
            return reinterpret_cast< const IMAGE_NT_HEADERS* >( buffer.get() + offset );
        }

        inline std::string section_name( const IMAGE_SECTION_HEADER& header )
        {
            const auto* begin = reinterpret_cast< const char* >( header.Name );
            const auto* end = std::find( begin, begin + IMAGE_SIZEOF_SHORT_NAME, '\0' );
            return { begin, end };
        }
    }  // namespace detail

    inline module_t::module_t( const memory_factory& factory, const core::module_entry_t& entry )
        : memory_t( factory, entry.base_address, entry.base_size ),
          entry( entry ),
          entry_point_address( 0 ),
          nt_headers_offset( 0 ),
          section_count( 0 )
    {
        MODULEINFO info{};

        if ( !GetModuleInformation(
                 static_cast< HANDLE >( factory.p->handle->native ), reinterpret_cast< HMODULE >( entry.base_address ), &info, sizeof( info ) ) )
            throw core::error::from_win32( GetLastError() );

        entry_point_address = reinterpret_cast< std::uintptr_t >( info.EntryPoint );
        header_buffer = read( 0, 0x1000 );

        const auto* dos_header = reinterpret_cast< const IMAGE_DOS_HEADER* >( header_buffer.get() );
        nt_headers_offset = static_cast< std::uintptr_t >( dos_header->e_lfanew );
        section_count = detail::nt_headers( header_buffer, nt_headers_offset )->FileHeader.NumberOfSections;
    }

    inline std::string_view module_t::name() const noexcept
    {
        return entry.name;
    }

    inline std::uintptr_t module_t::entry_point() const noexcept
    {
        return entry_point_address;
    }

    inline std::string module_t::path() const noexcept
    {
        return entry.path;
    }

    inline const std::list< std::shared_ptr< module_t::export_t > >& module_t::exports() const
    {
        if ( !_exports.empty() )
            return _exports;

        const auto* nt_headers = detail::nt_headers( header_buffer, nt_headers_offset );
        const auto directory_header = nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

        if ( !directory_header.VirtualAddress || !directory_header.Size )
            return _exports;

        const auto expbuffer = read( directory_header.VirtualAddress, directory_header.Size );
        const auto rva_to_offset = [ directory_header ]( const std::uintptr_t rva )
        { return static_cast< std::uintptr_t >( rva - directory_header.VirtualAddress ); };

        const auto* export_directory =
            reinterpret_cast< const IMAGE_EXPORT_DIRECTORY* >( expbuffer.get() + rva_to_offset( directory_header.VirtualAddress ) );
        const auto* names = reinterpret_cast< const std::uint32_t* >( expbuffer.get() + rva_to_offset( export_directory->AddressOfNames ) );
        const auto* ordinals = reinterpret_cast< const std::uint16_t* >( expbuffer.get() + rva_to_offset( export_directory->AddressOfNameOrdinals ) );
        const auto* functions = reinterpret_cast< const std::uint32_t* >( expbuffer.get() + rva_to_offset( export_directory->AddressOfFunctions ) );

        for ( std::uint32_t index = 0; index < export_directory->NumberOfNames; ++index )
        {
            const auto ordinal = ordinals[ index ];
            const auto address = functions[ ordinal ];
            const auto* name = reinterpret_cast< const char* >( expbuffer.get() + rva_to_offset( names[ index ] ) );

            if ( address >= directory_header.VirtualAddress && address < directory_header.VirtualAddress + directory_header.Size )
            {
                const std::string forward = reinterpret_cast< const char* >( expbuffer.get() + rva_to_offset( address ) );
                const auto dot = forward.find( '.' );

                if ( dot == std::string::npos )
                    continue;

                const auto module_name = forward.substr( 0, dot );
                const auto export_name = forward.substr( dot + 1 );
                const auto module = factory->p->module_factory.fetch_module( module_name );

                if ( !module )
                    continue;

                const auto exp = module->fetch_export( export_name );

                if ( !exp )
                    continue;

                _exports.emplace_back( new export_t{ exp->module(), name, exp->rva, exp->ordinal() } );
                continue;
            }

            _exports.emplace_back( new export_t( shared_from_this(), name, address, ordinal ) );
        }

        return _exports;
    }

    inline std::shared_ptr< module_t::export_t > module_t::fetch_export( const std::string_view name ) const
    {
        for ( const auto& exp : exports() )
        {
            if ( exp->name() == name )
                return exp;
        }

        return nullptr;
    }

    inline core::result_t< std::shared_ptr< module_t::export_t > > module_t::try_fetch_export( const std::string_view name ) const noexcept
    {
        try
        {
            if ( auto result = fetch_export( name ) )
                return result;

            return core::unexpected_t{ core::error::from_user(
                core::user_error_type_t::export_not_found_t, "Failed to find export \"{}\" in module \"{}\"", name, this->name() ) };
        }
        catch ( const core::error& error )
        {
            return core::unexpected_t{ error };
        }
        catch ( const std::exception& exception )
        {
            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::operation_failed_t, "{}", exception.what() ) };
        }
        catch ( ... )
        {
            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::operation_failed_t, "Unknown failure" ) };
        }
    }

    inline const std::list< std::shared_ptr< module_t::section_t > >& module_t::sections() const
    {
        if ( !_sections.empty() )
            return _sections;

        const auto* nt_headers = detail::nt_headers( header_buffer, nt_headers_offset );
        const auto* section = IMAGE_FIRST_SECTION( nt_headers );

        for ( const auto index : std::views::iota( std::uint16_t{ 0 }, section_count ) )
        {
            const auto& header = section[ index ];
            const auto size = std::max( header.SizeOfRawData, header.Misc.VirtualSize );

            _sections.emplace_back( new section_t(
                shared_from_this(), detail::section_name( header ), address() + header.VirtualAddress, static_cast< std::size_t >( size ) ) );
        }

        return _sections;
    }

    inline std::shared_ptr< module_t::section_t > module_t::fetch_section( const std::string_view name ) const
    {
        for ( const auto& section : sections() )
        {
            if ( section->name() == name )
                return section;
        }

        return nullptr;
    }

    inline std::vector< std::shared_ptr< rtti::object_t > > module_t::fetch_objects( const std::string_view mangled ) const
    {
        std::vector< std::shared_ptr< rtti::object_t > > objects;
        const auto data = fetch_section( ".data" );
        const auto rdata = fetch_section( ".rdata" );

        if ( !data || !rdata )
            return objects;

        const auto result = data->find( patterns::pattern_t{ mangled } );

        if ( !result )
            return objects;

        const auto type_descriptor_address = *result - sizeof( std::uintptr_t ) * 2;
        const auto type_descriptor_rva = static_cast< std::int32_t >( type_descriptor_address - address() );
        const auto cross_references = rdata->find_all( type_descriptor_rva );

        for ( const auto reference : cross_references )
        {
            const auto col_address = reference - sizeof( std::uint32_t ) * 3;
            const auto col = factory->read< rtti::complete_object_locator_t >( col_address );

            if ( col.signature != 1 )
                continue;

            const auto col_reference = rdata->find( patterns::pattern_t{ col_address } );

            if ( !col_reference )
                continue;

            objects.emplace_back( new rtti::object_t( this, *col_reference + sizeof( std::uintptr_t ), col ) );
        }

        return objects;
    }

    inline const module_t::export_t& module_t::operator[]( const std::string_view name ) const
    {
        if ( const auto result = fetch_export( name ) )
            return *result;

        throw core::error::from_user(
            core::user_error_type_t::export_not_found_t, "Failed to find export \"{}\" in module \"{}\"", name, this->name() );
    }
}  // namespace wincpp::modules
