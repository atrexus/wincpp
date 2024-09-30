#include "wincpp/memory_factory.hpp"

#include <atomic>
#include <execution>

#include "wincpp/core/error.hpp"
#include "wincpp/patterns/scanner.hpp"
#include "wincpp/process.hpp"

namespace wincpp
{
    memory_factory::memory_factory( process_t* p, memory_type type ) noexcept : p( p ), type( type )
    {
    }

    std::shared_ptr< std::uint8_t[] > memory_factory::read( std::uintptr_t address, std::size_t size ) const noexcept
    {
        const auto buffer = std::shared_ptr< std::uint8_t[] >( new std::uint8_t[ size ] );

        switch ( type )
        {
            case wincpp::memory_type::local_t:
            {
                std::memcpy( buffer.get(), reinterpret_cast< void* >( address ), size );
                break;
            }
            case wincpp::memory_type::remote_t:
            {
                ReadProcessMemory( p->handle->native, reinterpret_cast< void* >( address ), buffer.get(), size, nullptr );
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
                std::memmove( reinterpret_cast< void* >( address ), buffer.get(), size );
                break;
            }
            case wincpp::memory_type::remote_t:
            {
                std::size_t written;

                if ( !WriteProcessMemory( p->handle->native, reinterpret_cast< void* >( address ), buffer.get(), size, &written ) )
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

        if ( !VirtualProtectEx( p->handle->native, reinterpret_cast< void* >( address ), size, new_flags.get(), &old_flags ) )
            throw core::error::from_win32( GetLastError() );

        return memory::protection_operation( new memory::protection_operation_t( address, size, new_flags, old_flags ), p->handle );
    }

    memory::working_set_information_t memory_factory::working_set_information( std::uintptr_t address ) const
    {
        PSAPI_WORKING_SET_EX_INFORMATION info{};
        info.VirtualAddress = reinterpret_cast< void* >( address );

        if ( !QueryWorkingSetEx( p->handle->native, &info, sizeof( info ) ) )
            throw core::error::from_win32( GetLastError() );

        return memory::working_set_information_t( info );
    }

    std::optional< std::uintptr_t > memory_factory::find_instance_of( const std::shared_ptr< modules::rtti::object_t >& object, bool parallelize )
        const
    {
        return find_instance_of(
            object, []( const memory::region_t& region ) { return true; }, parallelize );
    }

    std::optional< std::uintptr_t > memory_factory::find_instance_of(
        const std::shared_ptr< modules::rtti::object_t >& object,
        const region_compare& compare,
        bool parallelize ) const
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
                return;  // Early exit check

            const auto buffer = read( region.address(), region.size() );
            if ( !buffer )
                return;

            std::span< std::uint8_t > bytes( buffer.get(), region.size() );
            const auto result = patterns::scanner::find< patterns::scanner::algorithm_t::tbm_t >( bytes, object->vtable() );

            if ( result )
            {
                address = region.address() + *result;
                stop_source.request_stop();  // Signal other threads to stop
            }
        };

        if ( parallelize )
            std::for_each( std::execution::par_unseq, region_list.begin(), region_list.end(), lambda );
        else
            std::for_each( std::execution::unseq, region_list.begin(), region_list.end(), lambda );

        return address ? std::make_optional( address.load() ) : std::nullopt;
    }
}  // namespace wincpp