#include "process.hpp"

#include <psapi.h>

#include "core/snapshot.hpp"

namespace wincpp
{

    std::unique_ptr< process_t > process_t::open( std::string_view name, std::uint32_t access )
    {
        for ( const auto& proc : core::snapshot< core::snapshot_kind::process_t >::create() )
        {
            if ( proc.name == name )
            {
                const auto handle = OpenProcess( access, FALSE, proc.id );

                if ( !handle )
                    throw core::error::from_win32( GetLastError() );

                return std::unique_ptr< process_t >( new process_t( core::handle_t::create( handle ), proc.id, proc.name, memory_type::remote_t ) );
            }
        }

        return nullptr;
    }

    std::unique_ptr< process_t > process_t::open( std::uint32_t id, std::uint32_t access )
    {
        for ( const auto& proc : core::snapshot< core::snapshot_kind::process_t >::create() )
        {
            if ( proc.id == id )
            {
                const auto handle = OpenProcess( access, FALSE, id );

                if ( !handle )
                    throw core::error::from_win32( GetLastError() );

                return std::unique_ptr< process_t >( new process_t( core::handle_t::create( handle ), proc.id, proc.name, memory_type::remote_t ) );
            }
        }

        return nullptr;
    }

    std::unique_ptr< process_t > process_t::current()
    {
        const auto handle = core::handle_t::create( GetCurrentProcess(), false );

        // get the process id
        std::uint32_t id = GetProcessId( handle->native );

        // get the process name
        std::string name;
        name.resize( MAX_PATH );

        if ( !GetModuleFileNameExA( handle->native, nullptr, name.data(), MAX_PATH ) )
            throw core::error::from_win32( GetLastError() );

        return std::unique_ptr< process_t >( new process_t( handle, id, name, memory_type::local_t ) );
    }

    process_t::process_t( std::shared_ptr< core::handle_t > handle, std::uint32_t id, std::string_view name, memory_type type ) noexcept
        : handle( handle ),
          id( id ),
          name( name ),
          module_factory( this ),
          memory_factory( this, type )
    {
    }

}  // namespace wincpp