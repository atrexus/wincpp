#include "memory/memory.hpp"

#include "memory/region.hpp"

namespace wincpp::memory
{
    memory::region_list memory_t::regions() const
    {
        return factory.regions( _address, _address + _size );
    }
}  // namespace wincpp::memory