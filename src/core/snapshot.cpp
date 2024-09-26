#include "wincpp/core/snapshot.hpp"

namespace wincpp::core
{
    static inline void throw_if_fatal()
    {
        const auto last_error = GetLastError();

        if ( last_error != ERROR_NO_MORE_FILES )
        {
            throw error::from_win32( last_error );
        }
    }

    snapshot< snapshot_kind::process_t >::iterator::iterator( std::shared_ptr< handle_t > handle ) : handle( handle )
    {
        entry.dwSize = sizeof( PROCESSENTRY32 );

        if ( handle )
        {
            if ( !Process32First( handle->native, &entry ) )
                throw_if_fatal();
        }
    }

    process_entry_t& snapshot< snapshot_kind::process_t >::iterator::operator*() const noexcept
    {
        result.id = entry.th32ProcessID;
        result.threads = entry.cntThreads;
        result.parent_id = entry.th32ParentProcessID;
        result.priority = entry.pcPriClassBase;
        result.name = entry.szExeFile;

        return result;
    }

    inline process_entry_t* snapshot< snapshot_kind::process_t >::iterator::operator->() const noexcept
    {
        return &operator*();
    }

    snapshot< snapshot_kind::process_t >::iterator& snapshot< snapshot_kind::process_t >::iterator::operator++()
    {
        if ( !Process32Next( handle->native, &entry ) )
        {
            throw_if_fatal();
            handle.reset();
        }

        return *this;
    }

    bool snapshot< snapshot_kind::process_t >::iterator::operator==( const iterator& other ) const noexcept
    {
        return ( !handle && !other.handle ) || handle && other.handle && entry.th32ProcessID == other.entry.th32ProcessID;
    }

    bool snapshot< snapshot_kind::process_t >::iterator::operator!=( const iterator& other ) const noexcept
    {
        return !operator==( other );
    }

    snapshot< snapshot_kind::thread_t >::iterator::iterator( std::shared_ptr< handle_t > handle ) : handle( handle )
    {
        entry.dwSize = sizeof( THREADENTRY32 );

        if ( handle )
        {
            if ( !Thread32First( handle->native, &entry ) )
                throw_if_fatal();
        }
    }

    thread_entry_t& snapshot< snapshot_kind::thread_t >::iterator::operator*() const noexcept
    {
        result.id = entry.th32ThreadID;
        result.owner_id = entry.th32OwnerProcessID;
        result.base_priority = entry.tpBasePri;

        return result;
    }

    inline thread_entry_t* snapshot< snapshot_kind::thread_t >::iterator::operator->() const noexcept
    {
        return &operator*();
    }

    snapshot< snapshot_kind::thread_t >::iterator& snapshot< snapshot_kind::thread_t >::iterator::operator++()
    {
        if ( !Thread32Next( handle->native, &entry ) )
        {
            throw_if_fatal();
            handle.reset();
        }

        return *this;
    }

    bool snapshot< snapshot_kind::thread_t >::iterator::operator==( const iterator& other ) const noexcept
    {
        return ( !handle && !other.handle ) || handle && other.handle && entry.th32ThreadID == other.entry.th32ThreadID;
    }

    bool snapshot< snapshot_kind::thread_t >::iterator::operator!=( const iterator& other ) const noexcept
    {
        return !operator==( other );
    }

    module_entry_t::module_entry_t( const MODULEENTRY32& entry )
        : base_address( reinterpret_cast< std::uintptr_t >( entry.modBaseAddr ) ),
          base_size( entry.modBaseSize ),
          name( entry.szModule ),
          path( entry.szExePath ),
          process_id( entry.th32ProcessID ),
          usage_count( entry.GlblcntUsage )
    {
    }

    snapshot< snapshot_kind::module_t >::iterator::iterator( std::shared_ptr< handle_t > handle ) : handle( handle )
    {
        entry.dwSize = sizeof( MODULEENTRY32 );

        if ( handle )
        {
            if ( !Module32First( handle->native, &entry ) )
                throw_if_fatal();
        }
    }

    module_entry_t snapshot< snapshot_kind::module_t >::iterator::operator*() const noexcept
    {
        return entry;
    }

    snapshot< snapshot_kind::module_t >::iterator& snapshot< snapshot_kind::module_t >::iterator::operator++()
    {
        if ( !Module32Next( handle->native, &entry ) )
        {
            throw_if_fatal();
            handle.reset();
        }

        return *this;
    }

    bool snapshot< snapshot_kind::module_t >::iterator::operator==( const iterator& other ) const noexcept
    {
        return ( !handle && !other.handle ) || handle && other.handle && entry.modBaseAddr == other.entry.modBaseAddr;
    }

    bool snapshot< snapshot_kind::module_t >::iterator::operator!=( const iterator& other ) const noexcept
    {
        return !operator==( other );
    }

}  // namespace wincpp::core