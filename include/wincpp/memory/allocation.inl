#pragma once

#include "wincpp/memory/allocation.hpp"
#include "wincpp/memory_factory.hpp"

namespace wincpp::memory
{
    inline allocation_t::allocation_t( const memory_factory& mem, const std::uintptr_t address, const std::size_t size, const bool owns ) noexcept
        : memory_t( mem, address, size ),
          _owns( owns )
    {
    }

    inline void allocation_t::deleter::operator()( allocation_t* const allocation ) const noexcept
    {
        if ( allocation && allocation->_owns )
            allocation->factory->free( allocation->address() );

        delete allocation;
    }
}  // namespace wincpp::memory
