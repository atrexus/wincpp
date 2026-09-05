#pragma once

#include <algorithm>
#include <charconv>
#include <exception>
#include <limits>
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

    inline std::optional< module_t::export_directory_t > module_t::export_directory() const
    {
        const auto* nt_headers = detail::nt_headers( header_buffer, nt_headers_offset );
        if ( nt_headers->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_EXPORT )
            return std::nullopt;

        const auto data_directory = nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

        if ( !data_directory.VirtualAddress || !data_directory.Size )
            return std::nullopt;

        if ( data_directory.Size < sizeof( IMAGE_EXPORT_DIRECTORY ) || data_directory.VirtualAddress >= size() ||
             data_directory.Size > size() - data_directory.VirtualAddress )
            throw core::error::from_user( core::user_error_type_t::operation_failed_t, "Invalid export directory in module \"{}\"", name() );

        const auto directory = read< IMAGE_EXPORT_DIRECTORY >( data_directory.VirtualAddress );
        const auto table_fits = [ this ]( const std::uint32_t rva, const std::uint32_t count, const std::size_t entry_size )
        { return count == 0 || ( rva < size() && count <= ( size() - rva ) / entry_size ); };

        if ( ( directory.NumberOfFunctions && directory.NumberOfFunctions - 1 > std::numeric_limits< std::uint32_t >::max() - directory.Base ) ||
             ( directory.NumberOfNames && !directory.NumberOfFunctions ) ||
             !table_fits( directory.AddressOfFunctions, directory.NumberOfFunctions, sizeof( std::uint32_t ) ) ||
             !table_fits( directory.AddressOfNames, directory.NumberOfNames, sizeof( std::uint32_t ) ) ||
             !table_fits( directory.AddressOfNameOrdinals, directory.NumberOfNames, sizeof( std::uint16_t ) ) )
            throw core::error::from_user( core::user_error_type_t::operation_failed_t, "Invalid export tables in module \"{}\"", name() );

        return export_directory_t{ .virtual_address = data_directory.VirtualAddress,
                                   .size = data_directory.Size,
                                   .ordinal_base = directory.Base,
                                   .function_count = directory.NumberOfFunctions,
                                   .name_count = directory.NumberOfNames,
                                   .functions_rva = directory.AddressOfFunctions,
                                   .names_rva = directory.AddressOfNames,
                                   .name_ordinals_rva = directory.AddressOfNameOrdinals };
    }

    inline std::string module_t::read_export_string( const std::uint32_t rva, const std::uintptr_t end_rva ) const
    {
        const auto stop = std::min( end_rva, size() );
        if ( rva >= stop )
            throw core::error::from_user(
                core::user_error_type_t::operation_failed_t, "Export string RVA 0x{:X} is outside module \"{}\"", rva, name() );

        constexpr std::size_t chunk_size = 256;
        std::string result;
        auto offset = static_cast< std::uintptr_t >( rva );
        while ( offset < stop )
        {
            const auto length = std::min( chunk_size, stop - offset );
            const auto buffer = read( offset, length );
            if ( !buffer )
                throw core::error::from_win32( GetLastError() );

            const auto* begin = reinterpret_cast< const char* >( buffer.get() );
            const auto* end = std::find( begin, begin + length, '\0' );
            result.append( begin, end );
            if ( end != begin + length )
                return result;

            offset += length;
        }

        throw core::error::from_user( core::user_error_type_t::operation_failed_t, "Unterminated export string in module \"{}\"", name() );
    }

    inline std::optional< std::uint32_t > module_t::find_export_ordinal( const std::string_view name ) const
    {
        const auto directory = export_directory();
        if ( !directory || !directory->name_count )
            return std::nullopt;

        std::uint32_t lower = 0;
        std::uint32_t upper = directory->name_count;
        while ( lower < upper )
        {
            const auto index = lower + ( upper - lower ) / 2;
            const auto candidate_rva = read< std::uint32_t >( directory->names_rva + sizeof( std::uint32_t ) * index );
            const auto candidate = read_export_string( candidate_rva, size() );
            if ( candidate == name )
            {
                const auto function_index = read< std::uint16_t >( directory->name_ordinals_rva + sizeof( std::uint16_t ) * index );
                if ( function_index >= directory->function_count )
                    throw core::error::from_user(
                        core::user_error_type_t::operation_failed_t, "Invalid export name ordinal in module \"{}\"", this->name() );

                return directory->ordinal_base + function_index;
            }

            if ( candidate < name )
                lower = index + 1;
            else
                upper = index;
        }

        return std::nullopt;
    }

    inline std::optional< module_t::export_target_t > module_t::resolve_export_target(
        const std::uint32_t ordinal,
        std::vector< std::pair< std::uintptr_t, std::uint32_t > >& forwarder_chain ) const
    {
        const auto directory = export_directory();
        if ( !directory || !directory->function_count || ordinal < directory->ordinal_base ||
             ordinal - directory->ordinal_base >= directory->function_count )
            return std::nullopt;

        const auto function_index = ordinal - directory->ordinal_base;
        const auto function_rva = read< std::uint32_t >( directory->functions_rva + sizeof( std::uint32_t ) * function_index );
        if ( !function_rva )
            return std::nullopt;

        const auto export_end = static_cast< std::uintptr_t >( directory->virtual_address ) + directory->size;

        if ( function_rva < directory->virtual_address || function_rva >= export_end )
        {
            if ( function_rva >= size() )
                throw core::error::from_user(
                    core::user_error_type_t::operation_failed_t, "Export RVA 0x{:X} is outside module \"{}\"", function_rva, this->name() );
            return export_target_t{ .module = shared_from_this(), .rva = function_rva };
        }

        const auto key = std::pair{ address(), ordinal };
        if ( std::ranges::find( forwarder_chain, key ) != forwarder_chain.end() )
            throw core::error::from_user(
                core::user_error_type_t::operation_failed_t, "Circular export forwarder for ordinal {} in module \"{}\"", ordinal, this->name() );
        forwarder_chain.push_back( key );

        const auto forwarder = read_export_string( function_rva, export_end );
        const auto separator = forwarder.find_last_of( '.' );
        if ( separator == std::string::npos || separator == 0 || separator + 1 == forwarder.size() )
            throw core::error::from_user(
                core::user_error_type_t::operation_failed_t, "Malformed export forwarder \"{}\" in module \"{}\"", forwarder, this->name() );

        const auto target_module = factory->p->module_factory.fetch_module( std::string_view( forwarder ).substr( 0, separator ), this->name() );
        if ( !target_module )
            throw core::error::from_user(
                core::user_error_type_t::module_not_found_t, "Failed to resolve forwarder \"{}\" from module \"{}\"", forwarder, this->name() );

        const auto target_symbol = std::string_view( forwarder ).substr( separator + 1 );
        std::optional< export_target_t > resolved_target;
        if ( target_symbol.starts_with( '#' ) )
        {
            std::uint32_t target_ordinal{};
            const auto [ pointer, error ] = std::from_chars( target_symbol.data() + 1, target_symbol.data() + target_symbol.size(), target_ordinal );
            if ( error != std::errc{} || pointer != target_symbol.data() + target_symbol.size() )
                throw core::error::from_user( core::user_error_type_t::operation_failed_t, "Malformed ordinal forwarder \"{}\"", forwarder );

            resolved_target = target_module->resolve_export_target( target_ordinal, forwarder_chain );
        }
        else
        {
            if ( const auto target_ordinal = target_module->find_export_ordinal( target_symbol ) )
                resolved_target = target_module->resolve_export_target( *target_ordinal, forwarder_chain );
        }

        if ( !resolved_target )
            throw core::error::from_user(
                core::user_error_type_t::export_not_found_t, "Failed to resolve forwarder \"{}\" from module \"{}\"", forwarder, this->name() );

        return resolved_target;
    }

    inline std::shared_ptr< module_t::export_t > module_t::resolve_export( const std::uint32_t ordinal, const std::string_view name ) const
    {
        std::vector< std::pair< std::uintptr_t, std::uint32_t > > forwarder_chain;
        const auto target = resolve_export_target( ordinal, forwarder_chain );
        if ( !target )
            return nullptr;

        return std::shared_ptr< export_t >( new export_t( target->module, name, target->rva, ordinal ) );
    }

    inline const std::list< std::shared_ptr< module_t::export_t > >& module_t::exports() const
    {
        if ( _exports_loaded )
            return _exports;

        const auto directory = export_directory();
        if ( !directory || !directory->function_count )
        {
            _exports_loaded = true;
            return _exports;
        }

        std::list< std::shared_ptr< export_t > > parsed_exports;
        std::vector< bool > named_functions( directory->function_count );

        for ( std::uint32_t index = 0; index < directory->name_count; ++index )
        {
            const auto function_index = read< std::uint16_t >( directory->name_ordinals_rva + sizeof( std::uint16_t ) * index );
            if ( function_index >= directory->function_count )
                throw core::error::from_user( core::user_error_type_t::operation_failed_t, "Invalid export name ordinal in module \"{}\"", name() );

            named_functions[ function_index ] = true;
            const auto export_name = read_export_string( read< std::uint32_t >( directory->names_rva + sizeof( std::uint32_t ) * index ), size() );
            if ( const auto exp = resolve_export( directory->ordinal_base + function_index, export_name ) )
                parsed_exports.emplace_back( exp );
        }

        for ( std::uint32_t index = 0; index < directory->function_count; ++index )
        {
            if ( named_functions[ index ] )
                continue;

            const auto ordinal = directory->ordinal_base + index;
            if ( const auto exp = resolve_export( ordinal, {} ) )
                parsed_exports.emplace_back( exp );
        }

        _exports = std::move( parsed_exports );
        _exports_loaded = true;
        return _exports;
    }

    inline std::shared_ptr< module_t::export_t > module_t::fetch_export( const std::string_view name ) const
    {
        const auto ordinal = find_export_ordinal( name );
        if ( !ordinal )
            return nullptr;

        return resolve_export( *ordinal, name );
    }

    inline std::shared_ptr< module_t::export_t > module_t::fetch_export( const std::uint32_t ordinal ) const
    {
        return resolve_export( ordinal, {} );
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

    inline core::result_t< std::shared_ptr< module_t::export_t > > module_t::try_fetch_export( const std::uint32_t ordinal ) const noexcept
    {
        try
        {
            if ( auto result = fetch_export( ordinal ) )
                return result;

            return core::unexpected_t{ core::error::from_user(
                core::user_error_type_t::export_not_found_t, "Failed to find export ordinal {} in module \"{}\"", ordinal, this->name() ) };
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
