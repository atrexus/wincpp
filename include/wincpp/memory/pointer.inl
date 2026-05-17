#pragma once

#include "wincpp/memory/pointer.hpp"
#include "wincpp/memory/region.hpp"
#include "wincpp/memory_factory.hpp"

namespace wincpp::memory
{
    template< typename T >
    template< typename U >
    pointer_t< T >::value_t< U >::value_t( const std::uintptr_t address, const memory_factory& factory ) noexcept
        : address( address ),
          factory( &factory )
    {
    }

    template< typename T >
    template< typename U >
    pointer_t< T >::value_t< U >::operator T() const
    {
        return factory->read< T >( address );
    }

    template< typename T >
    template< typename U >
    const pointer_t< T >::template value_t< U >& pointer_t< T >::value_t< U >::operator=( const T& value ) const
    {
        factory->write< T >( address, value );
        return *this;
    }

    template< typename T >
    template< typename U >
    pointer_t< T >::value_t< U >::operator pointer_t< U >() const
    {
        return pointer_t< U >( factory->read< T >( address ), *factory );
    }

    template< typename T >
    pointer_t< T >::pointer_t( const std::uintptr_t address, const memory_factory& factory ) noexcept
        : memory_t( factory, address, sizeof( T ) ),
          value( address, factory )
    {
    }

    template< typename T >
    template< typename U >
    pointer_t< T >::operator pointer_t< U >() const
    {
        return pointer_t< U >( value.address, *value.factory );
    }

    template< typename T >
    pointer_t< T >::template value_t< T > pointer_t< T >::operator*() const noexcept
    {
        return value;
    }

    template< typename T >
    pointer_t< T >::operator std::uintptr_t() const noexcept
    {
        return value.address;
    }

    template< typename T >
    pointer_t< T >::operator bool() const noexcept
    {
        if ( value.address == 0 )
            return false;

        for ( const auto& region : value.factory->regions() )
        {
            if ( region.contains( value.address ) )
                return true;
        }

        return false;
    }

    template< typename T >
    template< std::integral U >
    pointer_t< T > pointer_t< T >::operator+( const U offset ) const noexcept
    {
        return pointer_t< T >( value.address + static_cast< std::uintptr_t >( offset ), *value.factory );
    }
}  // namespace wincpp::memory
