#pragma once

#include <algorithm>
#include <atomic>
#include <cstring>
#include <execution>
#include <limits>
#include <span>
#include <stop_token>
#include <type_traits>
#include <vector>

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/memory/allocation.hpp"
#include "wincpp/memory/pointer.hpp"
#include "wincpp/memory/region.hpp"
#include "wincpp/memory_factory.hpp"
#include "wincpp/patterns/scanner.hpp"
#include "wincpp/process.hpp"

namespace wincpp
{
    inline memory_factory::memory_factory( process_t* const p, const memory_type type ) noexcept : p( p ), type( type )
    {
    }

    inline bool memory_factory::read( const std::uintptr_t address, const std::size_t size, std::uint8_t* const buffer ) const noexcept
    {
        switch ( type )
        {
            case memory_type::local_t:
            {
                std::memcpy( buffer, reinterpret_cast< const void* >( address ), size );
                break;
            }
            case memory_type::remote_t:
            {
                SIZE_T read{};

                if ( !ReadProcessMemory(
                         static_cast< HANDLE >( p->handle->native ), reinterpret_cast< const void* >( address ), buffer, size, &read ) ||
                     read != size )
                    return false;

                break;
            }
        }

        return true;
    }

    inline bool memory_factory::read( const std::uintptr_t address, const std::span< std::byte > buffer ) const noexcept
    {
        return read( address, buffer.size(), reinterpret_cast< std::uint8_t* >( buffer.data() ) );
    }

    inline std::shared_ptr< std::uint8_t[] > memory_factory::read( const std::uintptr_t address, const std::size_t size ) const noexcept
    {
        auto buffer = std::make_shared_for_overwrite< std::uint8_t[] >( size );

        if ( read( address, size, buffer.get() ) )
            return buffer;

        return nullptr;
    }

    template< typename T >
        requires std::is_trivially_copyable_v< T >
    T memory_factory::read( const std::uintptr_t address ) const
    {
        const auto buffer = read( address, sizeof( T ) );

        if ( !buffer )
            throw core::error::from_win32( GetLastError() );

        T value{};
        std::memcpy( &value, buffer.get(), sizeof( T ) );
        return value;
    }

    inline std::string memory_factory::read_string( const std::uintptr_t address ) const
    {
        return read_string( address, buffer_size );
    }

    inline std::string memory_factory::read_string( const std::uintptr_t address, const std::size_t max_length ) const
    {
        if ( max_length == 0 )
            return {};

        std::vector< char > buffer( max_length );

        if ( !read( address, max_length, reinterpret_cast< std::uint8_t* >( buffer.data() ) ) )
            throw core::error::from_win32( GetLastError() );

        const auto end = std::ranges::find( buffer, '\0' );
        return { buffer.begin(), end };
    }

    inline std::wstring memory_factory::read_wstring( const std::uintptr_t address, const std::size_t max_length ) const
    {
        if ( max_length == 0 )
            return {};

        if ( max_length > std::numeric_limits< std::size_t >::max() / sizeof( wchar_t ) )
            throw core::error::from_user( core::user_error_type_t::memory_out_of_range_t, "Wide string length {} is too large", max_length );

        std::vector< wchar_t > buffer( max_length );
        const auto bytes = buffer.size() * sizeof( wchar_t );

        if ( !read( address, bytes, reinterpret_cast< std::uint8_t* >( buffer.data() ) ) )
            throw core::error::from_win32( GetLastError() );

        const auto end = std::ranges::find( buffer, L'\0' );
        return { buffer.begin(), end };
    }

    inline std::size_t memory_factory::write( const std::uintptr_t address, const std::shared_ptr< std::uint8_t[] >& buffer, const std::size_t size )
        const noexcept
    {
        return write( address, buffer.get(), size );
    }

    inline std::size_t memory_factory::write( const std::uintptr_t address, const std::uint8_t* const buffer, const std::size_t size ) const noexcept
    {
        switch ( type )
        {
            case memory_type::local_t:
            {
                std::memmove( reinterpret_cast< void* >( address ), buffer, size );
                break;
            }
            case memory_type::remote_t:
            {
                SIZE_T written{};
                WriteProcessMemory( static_cast< HANDLE >( p->handle->native ), reinterpret_cast< void* >( address ), buffer, size, &written );
                return written;
            }
        }

        return size;
    }

    inline std::size_t memory_factory::write( const std::uintptr_t address, const std::span< const std::byte > buffer ) const noexcept
    {
        return write( address, reinterpret_cast< const std::uint8_t* >( buffer.data() ), buffer.size() );
    }

    template< typename T >
        requires std::is_trivially_copyable_v< T >
    void memory_factory::write( const std::uintptr_t address, const T& value ) const
    {
        write( address, reinterpret_cast< const std::uint8_t* >( &value ), sizeof( T ) );
    }

    inline void memory_factory::write_string( const std::uintptr_t address, const std::string_view value ) const
    {
        std::vector< std::uint8_t > buffer( value.begin(), value.end() );
        buffer.push_back( 0 );
        write( address, buffer.data(), buffer.size() );
    }

    inline void memory_factory::write_wstring( const std::uintptr_t address, const std::wstring_view value ) const
    {
        if ( value.size() >= std::numeric_limits< std::size_t >::max() / sizeof( wchar_t ) )
            throw core::error::from_user( core::user_error_type_t::memory_out_of_range_t, "Wide string length {} is too large", value.size() );

        std::vector< wchar_t > buffer( value.begin(), value.end() );
        buffer.push_back( L'\0' );
        write( address, reinterpret_cast< const std::uint8_t* >( buffer.data() ), buffer.size() * sizeof( wchar_t ) );
    }

    inline memory::pointer_t< std::uintptr_t > memory_factory::operator[]( const std::uintptr_t address ) const
    {
        return memory::pointer_t< std::uintptr_t >( address, *this );
    }

    inline memory::region_list memory_factory::regions( const std::uintptr_t start, const std::uintptr_t stop ) const
    {
        return memory::region_list( p, start, stop );
    }

    inline std::optional< memory::region_t > memory_factory::query( const std::uintptr_t address ) const noexcept
    {
        MEMORY_BASIC_INFORMATION mbi{};

        if ( VirtualQueryEx( static_cast< HANDLE >( p->handle->native ), reinterpret_cast< LPCVOID >( address ), &mbi, sizeof( mbi ) ) == 0 )
            return std::nullopt;

        const auto base_address = reinterpret_cast< std::uintptr_t >( mbi.BaseAddress );
        const auto region_size = static_cast< std::size_t >( mbi.RegionSize );
        const auto state = static_cast< memory::region_t::state_t >( mbi.State );
        const auto memory_kind = static_cast< memory::region_t::type_t >( mbi.Type );
        const auto protection = memory::protection_flags_t( static_cast< std::uint32_t >( mbi.Protect ) );

        return memory::region_t{ *this, base_address, region_size, state, memory_kind, protection };
    }

    inline memory::protection_operation memory_factory::protect(
        const std::uintptr_t address,
        const std::size_t size,
        const memory::protection_flags_t new_flags,
        const bool scoped ) const
    {
        DWORD old_flags{};

        if ( !VirtualProtectEx(
                 static_cast< HANDLE >( p->handle->native ), reinterpret_cast< void* >( address ), size, new_flags.get(), &old_flags ) )
            throw core::error::from_win32( GetLastError() );

        return { new memory::protection_operation_t( address, size, new_flags, memory::protection_flags_t( old_flags ) ),
                 memory::protection_operation_t::deleter{ p->handle, scoped } };
    }

    inline memory::working_set_information_t memory_factory::working_set_information( const std::uintptr_t address ) const
    {
        PSAPI_WORKING_SET_EX_INFORMATION info{};
        info.VirtualAddress = reinterpret_cast< void* >( address );

        if ( !QueryWorkingSetEx( static_cast< HANDLE >( p->handle->native ), &info, sizeof( info ) ) )
            throw core::error::from_win32( GetLastError() );

        return { .virtual_address = reinterpret_cast< std::uintptr_t >( info.VirtualAddress ),
                 .valid = info.VirtualAttributes.Valid != 0,
                 .share_count = info.VirtualAttributes.ShareCount,
                 .protection = memory::protection_flags_t( static_cast< std::uint32_t >( info.VirtualAttributes.Win32Protection ) ) };
    }

    inline std::optional< std::uintptr_t > memory_factory::find_instance_of(
        const std::shared_ptr< modules::rtti::object_t >& object,
        const bool parallelize ) const
    {
        return find_instance_of( object, []( const memory::region_t& ) { return true; }, parallelize );
    }

    inline std::optional< std::uintptr_t > memory_factory::find_instance_of(
        const std::shared_ptr< modules::rtti::object_t >& object,
        const region_compare& compare,
        const bool parallelize ) const
    {
        std::vector< memory::region_t > region_list;

        for ( const auto& region : regions() )
        {
            if ( region.protection() != memory::protection_flags_t::readwrite || region.type() != memory::region_t::type_t::private_t ||
                 region.state() != memory::region_t::state_t::commit_t )
                continue;

            if ( compare( region ) )
                region_list.push_back( region );
        }

        std::atomic< std::uintptr_t > address = 0;
        std::stop_source stop_source;

        const auto lambda = [ & ]( const memory::region_t& region )
        {
            if ( stop_source.stop_requested() )
                return;

            const auto buffer = read( region.address(), region.size() );

            if ( !buffer )
                return;

            const auto bytes = std::span< const std::uint8_t >( buffer.get(), region.size() );
            const auto result = patterns::scanner::find< patterns::scanner::algorithm_t::tbm_t >( bytes, object->vtable() );

            if ( result )
            {
                address = region.address() + *result;
                stop_source.request_stop();
            }
        };

        if ( parallelize )
            std::for_each( std::execution::par_unseq, region_list.begin(), region_list.end(), lambda );
        else
            std::for_each( std::execution::unseq, region_list.begin(), region_list.end(), lambda );

        if ( address )
            return address.load();

        return std::nullopt;
    }

    inline void memory_factory::free( const std::uintptr_t address ) const
    {
        switch ( type )
        {
            case memory_type::local_t:
            {
                VirtualFree( reinterpret_cast< void* >( address ), 0, MEM_RELEASE );
                break;
            }
            case memory_type::remote_t:
            {
                VirtualFreeEx( static_cast< HANDLE >( p->handle->native ), reinterpret_cast< void* >( address ), 0, MEM_RELEASE );
                break;
            }
        }
    }

    inline std::shared_ptr< memory::allocation_t >
    memory_factory::allocate( const std::size_t size, const memory::protection_flags_t protection, const bool owns ) const
    {
        switch ( type )
        {
            case memory_type::local_t:
            {
                const auto address = reinterpret_cast< std::uintptr_t >( VirtualAlloc( nullptr, size, MEM_COMMIT | MEM_RESERVE, protection.get() ) );

                if ( !address )
                    throw core::error::from_win32( GetLastError() );

                return { new memory::allocation_t( *this, address, size, owns ), memory::allocation_t::deleter{} };
            }
            case memory_type::remote_t:
            {
                const auto address = reinterpret_cast< std::uintptr_t >(
                    VirtualAllocEx( static_cast< HANDLE >( p->handle->native ), nullptr, size, MEM_COMMIT | MEM_RESERVE, protection.get() ) );

                if ( !address )
                    throw core::error::from_win32( GetLastError() );

                return { new memory::allocation_t( *this, address, size, owns ), memory::allocation_t::deleter{} };
            }
        }

        return nullptr;
    }

    template< typename T >
    std::shared_ptr< memory::allocation_t > memory_factory::allocate( const memory::protection_flags_t protection, const bool owns ) const
    {
        return allocate( sizeof( T ), protection, owns );
    }
}  // namespace wincpp
