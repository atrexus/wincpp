#include "wincpp/memory/region.hpp"
#include "wincpp/patterns/scanner.hpp"

namespace wincpp::memory
{
    working_set_information_t::working_set_information_t( const PSAPI_WORKING_SET_EX_INFORMATION &info ) noexcept
        : virtual_address( reinterpret_cast< std::uintptr_t >( info.VirtualAddress ) ),
          valid( info.VirtualAttributes.Valid ),
          share_count( info.VirtualAttributes.ShareCount ),
          protection( info.VirtualAttributes.Win32Protection )
    {
    }

    memory_t::memory_t( const memory_factory &factory, std::uintptr_t address, std::size_t size ) noexcept
        : factory( factory ),
          _address( address ),
          _size( size )
    {
    }

    working_set_information_t memory_t::working_set_information() const
    {
        return factory.working_set_information( _address );
    }

    memory::region_list memory_t::regions() const
    {
        return factory.regions( _address, _address + _size );
    }

    bool memory_t::is_valid_region( const memory::region_t &region ) const noexcept
    {
        // If the region exceeds the bounds of the memory object, doesn't consist of committed memory, or isn't readable, skip it.
        return region.address() < _address + _size && region.state() == memory::region_t::state_t::commit_t &&
               !region.protection().has( memory::protection_t::noaccess_t ) && !region.protection().has( memory::protection_t::guard_t );
    }

    std::optional< std::uintptr_t > memory_t::find( const patterns::pattern_t &pattern ) const noexcept
    {
        for ( const auto &region : regions() )
        {
            if ( !is_valid_region( region ) )
                break;

            const auto buffer = factory.read( region.address(), region.size() );

            std::span< std::uint8_t > bytes( buffer.get(), region.size() );

            if ( const auto result = patterns::scanner::find< patterns::scanner::algorithm_t::bmh_t >( bytes, pattern ) )
                return region.address() + *result;
        }

        return std::nullopt;
    }

    std::vector< std::uintptr_t > memory_t::find_all( const patterns::pattern_t &pattern ) const noexcept
    {
        std::vector< std::uintptr_t > results;

        for ( const auto &region : regions() )
        {
            if ( !is_valid_region( region ) )
                break;

            const auto buffer = factory.read( region.address(), region.size() );

            std::span< std::uint8_t > bytes( buffer.get(), region.size() );

            for ( const auto &result : patterns::scanner::find_all< patterns::scanner::algorithm_t::bmh_t >( bytes, pattern ) )
                results.push_back( region.address() + result );
        }

        return results;
    }
}  // namespace wincpp::memory