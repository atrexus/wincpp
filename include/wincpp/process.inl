#pragma once

#include <exception>
#include <utility>

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/process.hpp"

namespace wincpp
{
    namespace detail
    {
        template< typename Exception >
        inline core::unexpected_t make_unexpected_operation_failure( const Exception& exception ) noexcept
        {
            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::operation_failed_t, "{}", exception.what() ) };
        }
    }  // namespace detail

    inline std::unique_ptr< process_t > process_t::open( const std::string_view name, const core::process_access_t access )
    {
        for ( const auto& proc : core::snapshot< core::snapshot_kind::process_t >::create() )
        {
            if ( proc.name == name )
            {
                const auto handle = OpenProcess( std::to_underlying( access ), FALSE, proc.id );

                if ( !handle )
                    throw core::error::from_win32( GetLastError() );

                return std::unique_ptr< process_t >( new process_t( core::handle_t::create( handle ), proc, memory_type::remote_t ) );
            }
        }

        throw core::error::from_user( core::user_error_type_t::process_not_found_t, "Failed to find process \"{}\"", name );
    }

    inline core::result_t< std::unique_ptr< process_t > > process_t::try_open(
        const std::string_view name,
        const core::process_access_t access ) noexcept
    {
        try
        {
            return open( name, access );
        }
        catch ( const core::error& error )
        {
            return core::unexpected_t{ error };
        }
        catch ( const std::exception& exception )
        {
            return detail::make_unexpected_operation_failure( exception );
        }
        catch ( ... )
        {
            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::operation_failed_t, "Unknown failure" ) };
        }
    }

    inline std::unique_ptr< process_t > process_t::open( const std::uint32_t id, const core::process_access_t access )
    {
        for ( const auto& proc : core::snapshot< core::snapshot_kind::process_t >::create() )
        {
            if ( proc.id == id )
            {
                const auto handle = OpenProcess( std::to_underlying( access ), FALSE, id );

                if ( !handle )
                    throw core::error::from_win32( GetLastError() );

                return std::unique_ptr< process_t >( new process_t( core::handle_t::create( handle ), proc, memory_type::remote_t ) );
            }
        }

        throw core::error::from_user( core::user_error_type_t::process_not_found_t, "Failed to find process with id {}", id );
    }

    inline core::result_t< std::unique_ptr< process_t > > process_t::try_open( const std::uint32_t id, const core::process_access_t access ) noexcept
    {
        try
        {
            return open( id, access );
        }
        catch ( const core::error& error )
        {
            return core::unexpected_t{ error };
        }
        catch ( const std::exception& exception )
        {
            return detail::make_unexpected_operation_failure( exception );
        }
        catch ( ... )
        {
            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::operation_failed_t, "Unknown failure" ) };
        }
    }

    inline std::unique_ptr< process_t > process_t::current()
    {
        const auto handle = core::handle_t::create( GetCurrentProcess(), false );
        const auto id = GetCurrentProcessId();

        char name[ MAX_PATH ]{};

        if ( !GetModuleBaseNameA( static_cast< HANDLE >( handle->native ), nullptr, name, MAX_PATH ) )
            throw core::error::from_win32( GetLastError() );

        return std::unique_ptr< process_t >( new process_t(
            handle, core::process_entry_t{ .id = id, .threads = 0, .parent_id = 0, .priority = 0, .name = name }, memory_type::local_t ) );
    }

    inline core::result_t< std::unique_ptr< process_t > > process_t::try_current() noexcept
    {
        try
        {
            return current();
        }
        catch ( const core::error& error )
        {
            return core::unexpected_t{ error };
        }
        catch ( const std::exception& exception )
        {
            return detail::make_unexpected_operation_failure( exception );
        }
        catch ( ... )
        {
            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::operation_failed_t, "Unknown failure" ) };
        }
    }

    constexpr std::uint32_t process_t::id() const noexcept
    {
        return entry.id;
    }

    constexpr std::string_view process_t::name() const noexcept
    {
        return entry.name;
    }

    inline process_t::process_t( std::shared_ptr< core::handle_t > handle, const core::process_entry_t& entry, const memory_type type ) noexcept
        : module_factory( this ),
          memory_factory( this, type ),
          window_factory( this ),
          thread_factory( this ),
          handle( std::move( handle ) ),
          entry( entry )
    {
    }
}  // namespace wincpp
