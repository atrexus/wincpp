#pragma once

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/memory/region.hpp"
#include "wincpp/process.hpp"

namespace wincpp::memory
{
    inline region_t::region_t(
        const memory_factory& factory,
        const std::uintptr_t address,
        const std::size_t size,
        const state_t state,
        const type_t type,
        const protection_flags_t protection ) noexcept
        : memory_t( factory, address, size ),
          _state( state ),
          _type( type ),
          _protection( protection )
    {
    }

    inline region_t::state_t region_t::state() const noexcept
    {
        return _state;
    }

    inline region_t::type_t region_t::type() const noexcept
    {
        return _type;
    }

    inline protection_flags_t region_t::protection() const noexcept
    {
        return _protection;
    }

    inline region_list::region_list( process_t* const process, const std::uintptr_t start, const std::uintptr_t stop ) noexcept
        : process( process ),
          start( start ),
          stop( stop )
    {
    }

    inline region_list::iterator region_list::begin() const
    {
        return iterator( process, start, stop );
    }

    inline region_list::iterator region_list::end() const noexcept
    {
        return iterator( process, stop, stop );
    }

    inline region_list::iterator::iterator( process_t* const process, const std::uintptr_t address, const std::uintptr_t stop )
        : process( process ),
          address( address ),
          stop( stop )
    {
        if ( address < stop )
            read_current();
    }

    inline region_t region_list::iterator::operator*() const
    {
        return *current;
    }

    inline region_list::iterator& region_list::iterator::operator++()
    {
        if ( !current )
            return *this;

        address = current->address() + current->size();

        if ( address >= stop )
        {
            current.reset();
            address = stop;
            return *this;
        }

        read_current();
        return *this;
    }

    inline bool region_list::iterator::operator==( const iterator& other ) const noexcept
    {
        return process == other.process && address == other.address && current.has_value() == other.current.has_value();
    }

    inline bool region_list::iterator::operator!=( const iterator& other ) const noexcept
    {
        return !( *this == other );
    }

    inline void region_list::iterator::read_current()
    {
        const auto region = process->memory_factory.query( address );

        if ( !region )
            throw core::error::from_win32( GetLastError() );

        current = region;
        address = current->address();
    }
}  // namespace wincpp::memory
