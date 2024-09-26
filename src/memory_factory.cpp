#include "wincpp/memory_factory.hpp"

#include "wincpp/core/error.hpp"
#include "wincpp/process.hpp"

namespace wincpp
{
    memory_factory::memory_factory( process_t *p, memory_type type ) noexcept : p( p ), type( type )
    {
    }

    std::shared_ptr< std::uint8_t[] > memory_factory::read( std::uintptr_t address, std::size_t size ) const
    {
        const auto buffer = std::shared_ptr< std::uint8_t[] >( new std::uint8_t[ size ] );

        switch ( type )
        {
            case wincpp::memory_type::local_t:
            {
                std::memcpy( buffer.get(), reinterpret_cast< void * >( address ), size );
                break;
            }
            case wincpp::memory_type::remote_t:
            {
                if ( !ReadProcessMemory( p->handle->native, reinterpret_cast< void * >( address ), buffer.get(), size, nullptr ) )
                    throw core::error::from_win32( GetLastError() );
                break;
            }
        }

        return buffer;
    }

    std::size_t memory_factory::write( std::uintptr_t address, std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) const
    {
        switch ( type )
        {
            case wincpp::memory_type::local_t:
            {
                std::memmove( reinterpret_cast< void * >( address ), buffer.get(), size );
                break;
            }
            case wincpp::memory_type::remote_t:
            {
                std::size_t written;

                if ( !WriteProcessMemory( p->handle->native, reinterpret_cast< void * >( address ), buffer.get(), size, &written ) )
                    throw core::error::from_win32( GetLastError() );

                return written;
            }
        }

        return size;
    }

    memory::pointer_t< std::uintptr_t > memory_factory::operator[]( std::uintptr_t address ) const
    {
        return memory::pointer_t< std::uintptr_t >( address, *this );
    }

    memory::region_list wincpp::memory_factory::regions( std::uintptr_t start, std::uintptr_t stop ) const
    {
        return memory::region_list( p, start, stop );
    }

    memory::protection_operation memory_factory::protect( std::uintptr_t address, std::size_t size, memory::protection_flags_t new_flags ) const
    {
        DWORD old_flags;

        if ( !VirtualProtectEx( p->handle->native, reinterpret_cast< void * >( address ), size, new_flags.get(), &old_flags ) )
            throw core::error::from_win32( GetLastError() );

        return memory::protection_operation( new memory::protection_operation_t( address, size, new_flags, old_flags ), p->handle );
    }

    memory::working_set_information_t memory_factory::working_set_information( std::uintptr_t address ) const
    {
        PSAPI_WORKING_SET_EX_INFORMATION info{};
        info.VirtualAddress = reinterpret_cast< void * >( address );

        if ( !QueryWorkingSetEx( p->handle->native, &info, sizeof( info ) ) )
            throw core::error::from_win32( GetLastError() );

        return memory::working_set_information_t( info );
    }
}  // namespace wincpp