#include "wincpp/memory/memory.hpp"
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

    patterns::scanner memory_t::scanner() const
    {
        return patterns::scanner( *this );
    }

}  // namespace wincpp::memory