#pragma once

#include <algorithm>
#include <cctype>
#include <utility>

#include "wincpp/core/error.hpp"
#include "wincpp/core/snapshot.hpp"
#include "wincpp/detail/win32.hpp"

namespace wincpp::core
{
    namespace detail
    {
        inline void throw_if_snapshot_failed()
        {
            const auto last_error = GetLastError();

            if ( last_error != ERROR_NO_MORE_FILES )
                throw error::from_win32( last_error );
        }

        inline std::shared_ptr< handle_t > create_snapshot_handle( const snapshot_kind kind, const std::uint32_t id )
        {
            const auto handle = CreateToolhelp32Snapshot( std::to_underlying( kind ), id );

            if ( handle == INVALID_HANDLE_VALUE || !handle )
                throw error::from_win32( GetLastError() );

            return handle_t::create( handle );
        }
    }  // namespace detail

    inline snapshot< snapshot_kind::process_t > snapshot< snapshot_kind::process_t >::create( const std::uint32_t id )
    {
        const auto handle = detail::create_snapshot_handle( snapshot_kind::process_t, id );
        std::vector< process_entry_t > entries;

        PROCESSENTRY32 entry{};
        entry.dwSize = sizeof( entry );

        if ( !Process32First( static_cast< HANDLE >( handle->native ), &entry ) )
        {
            detail::throw_if_snapshot_failed();
            return snapshot( std::move( entries ) );
        }

        do
        {
            entries.push_back(
                process_entry_t{ .id = entry.th32ProcessID,
                                 .threads = entry.cntThreads,
                                 .parent_id = entry.th32ParentProcessID,
                                 .priority = entry.pcPriClassBase,
                                 .name = entry.szExeFile } );
        } while ( Process32Next( static_cast< HANDLE >( handle->native ), &entry ) );

        detail::throw_if_snapshot_failed();
        return snapshot( std::move( entries ) );
    }

    inline snapshot< snapshot_kind::process_t >::snapshot( std::vector< value_type > entries ) : entries( std::move( entries ) )
    {
    }

    inline snapshot< snapshot_kind::process_t >::iterator snapshot< snapshot_kind::process_t >::begin() const noexcept
    {
        return entries.begin();
    }

    inline snapshot< snapshot_kind::process_t >::iterator snapshot< snapshot_kind::process_t >::end() const noexcept
    {
        return entries.end();
    }

    inline snapshot< snapshot_kind::thread_t > snapshot< snapshot_kind::thread_t >::create( const std::uint32_t id )
    {
        const auto handle = detail::create_snapshot_handle( snapshot_kind::thread_t, id );
        std::vector< thread_entry_t > entries;

        THREADENTRY32 entry{};
        entry.dwSize = sizeof( entry );

        if ( !Thread32First( static_cast< HANDLE >( handle->native ), &entry ) )
        {
            detail::throw_if_snapshot_failed();
            return snapshot( std::move( entries ) );
        }

        do
        {
            entries.push_back(
                thread_entry_t{ .id = entry.th32ThreadID,
                                .owner_id = entry.th32OwnerProcessID,
                                .base_priority = static_cast< std::uint32_t >( entry.tpBasePri ) } );
        } while ( Thread32Next( static_cast< HANDLE >( handle->native ), &entry ) );

        detail::throw_if_snapshot_failed();
        return snapshot( std::move( entries ) );
    }

    inline snapshot< snapshot_kind::thread_t >::snapshot( std::vector< value_type > entries ) : entries( std::move( entries ) )
    {
    }

    inline snapshot< snapshot_kind::thread_t >::iterator snapshot< snapshot_kind::thread_t >::begin() const noexcept
    {
        return entries.begin();
    }

    inline snapshot< snapshot_kind::thread_t >::iterator snapshot< snapshot_kind::thread_t >::end() const noexcept
    {
        return entries.end();
    }

    inline snapshot< snapshot_kind::module_t > snapshot< snapshot_kind::module_t >::create( const std::uint32_t id )
    {
        const auto handle = detail::create_snapshot_handle( snapshot_kind::module_t, id );
        std::vector< module_entry_t > entries;

        MODULEENTRY32 entry{};
        entry.dwSize = sizeof( entry );

        if ( !Module32First( static_cast< HANDLE >( handle->native ), &entry ) )
        {
            detail::throw_if_snapshot_failed();
            return snapshot( std::move( entries ) );
        }

        do
        {
            std::string name = entry.szModule;
            std::ranges::transform( name, name.begin(), []( const unsigned char ch ) { return static_cast< char >( std::tolower( ch ) ); } );

            entries.push_back(
                module_entry_t{ .process_id = entry.th32ProcessID,
                                .usage_count = entry.GlblcntUsage,
                                .base_address = reinterpret_cast< std::uintptr_t >( entry.modBaseAddr ),
                                .base_size = entry.modBaseSize,
                                .name = std::move( name ),
                                .path = entry.szExePath } );
        } while ( Module32Next( static_cast< HANDLE >( handle->native ), &entry ) );

        detail::throw_if_snapshot_failed();
        return snapshot( std::move( entries ) );
    }

    inline snapshot< snapshot_kind::module_t >::snapshot( std::vector< value_type > entries ) : entries( std::move( entries ) )
    {
    }

    inline snapshot< snapshot_kind::module_t >::iterator snapshot< snapshot_kind::module_t >::begin() const noexcept
    {
        return entries.begin();
    }

    inline snapshot< snapshot_kind::module_t >::iterator snapshot< snapshot_kind::module_t >::end() const noexcept
    {
        return entries.end();
    }
}  // namespace wincpp::core
