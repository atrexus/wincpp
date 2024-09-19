#include "memory/memory.hpp"
#include "memory/region.hpp"
#include "patterns/scanner.hpp"

namespace wincpp::memory
{
    memory_t::memory_t( const memory_factory &factory, std::uintptr_t address, std::size_t size ) noexcept
        : factory( factory ),
          _address( address ),
          _size( size )
    {
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