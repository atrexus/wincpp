#pragma once

#include <algorithm>
#include <limits>
#include <span>

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/memory/memory.hpp"
#include "wincpp/memory/region.hpp"
#include "wincpp/memory_factory.hpp"
#include "wincpp/patterns/scanner.hpp"

namespace wincpp::memory
{
    inline memory_t::memory_t( const memory_factory& mem, const std::uintptr_t address, const std::size_t size ) noexcept
        : factory( &mem ),
          _address( address ),
          _size( size )
    {
    }

    constexpr std::uintptr_t memory_t::address() const noexcept
    {
        return _address;
    }

    constexpr std::size_t memory_t::size() const noexcept
    {
        return _size;
    }

    constexpr bool memory_t::contains( const std::uintptr_t address ) const noexcept
    {
        return address >= _address && address - _address < _size;
    }

    inline working_set_information_t memory_t::working_set_information() const
    {
        return factory->working_set_information( _address );
    }

    inline std::shared_ptr< std::uint8_t[] > memory_t::read() const
    {
        return read( 0, _size );
    }

    inline void memory_t::read( std::uint8_t* const buffer ) const
    {
        if ( !factory->read( _address, _size, buffer ) )
            throw core::error::from_win32( GetLastError() );
    }

    inline void memory_t::read( const std::span< std::byte > buffer ) const
    {
        read( 0, buffer );
    }

    inline std::shared_ptr< std::uint8_t[] > memory_t::read( const std::uintptr_t offset, const std::size_t size ) const
    {
        return factory->read( checked_address( offset, size ), size );
    }

    inline void memory_t::read( const std::uintptr_t offset, const std::span< std::byte > buffer ) const
    {
        if ( !factory->read( checked_address( offset, buffer.size() ), buffer ) )
            throw core::error::from_win32( GetLastError() );
    }

    inline std::size_t memory_t::write( const std::uintptr_t offset, const std::shared_ptr< std::uint8_t[] >& buffer, const std::size_t size ) const
    {
        return factory->write( checked_address( offset, size ), buffer, size );
    }

    inline std::size_t memory_t::write( const std::uintptr_t offset, const std::uint8_t* const buffer, const std::size_t size ) const
    {
        return factory->write( checked_address( offset, size ), buffer, size );
    }

    inline std::size_t memory_t::write( const std::span< const std::byte > buffer ) const
    {
        return write( 0, buffer );
    }

    inline std::size_t memory_t::write( const std::uintptr_t offset, const std::span< const std::byte > buffer ) const
    {
        return factory->write( checked_address( offset, buffer.size() ), buffer );
    }

    template< typename T >
        requires std::is_trivially_copyable_v< T >
    T memory_t::read( const std::uintptr_t offset ) const
    {
        return factory->read< T >( checked_address( offset, sizeof( T ) ) );
    }

    template< typename T >
        requires std::is_trivially_copyable_v< T >
    void memory_t::write( const std::uintptr_t offset, const T& value ) const
    {
        factory->write< T >( checked_address( offset, sizeof( T ) ), value );
    }

    inline region_list memory_t::regions() const
    {
        const auto stop =
            _size > std::numeric_limits< std::uintptr_t >::max() - _address ? std::numeric_limits< std::uintptr_t >::max() : _address + _size;
        return factory->regions( _address, stop );
    }

    inline std::optional< std::uintptr_t > memory_t::find( const patterns::pattern_t& pattern ) const noexcept
    {
        try
        {
            return find( pattern, patterns::scanner::algorithm_t::naive_t );
        }
        catch ( ... )
        {
            return std::nullopt;
        }
    }

    inline std::optional< std::uintptr_t > memory_t::find( const patterns::pattern_t& pattern, const patterns::scanner::algorithm_t algorithm ) const
    {
        return find( pattern, algorithm, []( const memory::region_t& ) { return true; } );
    }

    inline std::optional< std::uintptr_t >
    memory_t::find( const patterns::pattern_t& pattern, const patterns::scanner::algorithm_t algorithm, const region_filter& filter ) const
    {
        for ( const auto& region : regions() )
        {
            if ( !is_valid_region( region ) || !filter( region ) )
                continue;

            const auto object_stop =
                _size > std::numeric_limits< std::uintptr_t >::max() - _address ? std::numeric_limits< std::uintptr_t >::max() : _address + _size;
            const auto region_stop = region.size() > std::numeric_limits< std::uintptr_t >::max() - region.address()
                                         ? std::numeric_limits< std::uintptr_t >::max()
                                         : region.address() + region.size();
            const auto scan_start = std::max( region.address(), _address );
            const auto scan_stop = std::min( region_stop, object_stop );

            if ( scan_start >= scan_stop )
                continue;

            const auto scan_size = scan_stop - scan_start;
            const auto buffer = factory->read( scan_start, scan_size );

            if ( !buffer )
                continue;

            const auto bytes = std::span< const std::uint8_t >( buffer.get(), scan_size );

            if ( const auto result = patterns::scanner::find( bytes, pattern, algorithm ) )
                return scan_start + *result;
        }

        return std::nullopt;
    }

    inline std::vector< std::uintptr_t > memory_t::find_all( const patterns::pattern_t& pattern ) const noexcept
    {
        try
        {
            return find_all( pattern, patterns::scanner::algorithm_t::naive_t );
        }
        catch ( ... )
        {
            return {};
        }
    }

    inline std::vector< std::uintptr_t > memory_t::find_all( const patterns::pattern_t& pattern, const patterns::scanner::algorithm_t algorithm )
        const
    {
        return find_all( pattern, algorithm, []( const memory::region_t& ) { return true; } );
    }

    inline std::vector< std::uintptr_t >
    memory_t::find_all( const patterns::pattern_t& pattern, const patterns::scanner::algorithm_t algorithm, const region_filter& filter ) const
    {
        std::vector< std::uintptr_t > results;

        for ( const auto& region : regions() )
        {
            if ( !is_valid_region( region ) || !filter( region ) )
                continue;

            const auto object_stop =
                _size > std::numeric_limits< std::uintptr_t >::max() - _address ? std::numeric_limits< std::uintptr_t >::max() : _address + _size;
            const auto region_stop = region.size() > std::numeric_limits< std::uintptr_t >::max() - region.address()
                                         ? std::numeric_limits< std::uintptr_t >::max()
                                         : region.address() + region.size();
            const auto scan_start = std::max( region.address(), _address );
            const auto scan_stop = std::min( region_stop, object_stop );

            if ( scan_start >= scan_stop )
                continue;

            const auto scan_size = scan_stop - scan_start;
            const auto buffer = factory->read( scan_start, scan_size );

            if ( !buffer )
                continue;

            const auto bytes = std::span< const std::uint8_t >( buffer.get(), scan_size );

            for ( const auto result : patterns::scanner::find_all( bytes, pattern, algorithm ) )
            {
                results.push_back( scan_start + result );
            }
        }

        return results;
    }

    inline protection_operation
    memory_t::protect( const std::uintptr_t offset, const std::size_t size, const protection_flags_t new_flags, const bool scoped ) const
    {
        return factory->protect( checked_address( offset, size ), size, new_flags, scoped );
    }

    inline protection_operation memory_t::protect( const protection_flags_t new_flags, const bool scoped ) const
    {
        return protect( 0, _size, new_flags, scoped );
    }

    inline const memory_factory& memory_t::memory() const noexcept
    {
        return *factory;
    }

    inline bool memory_t::is_valid_region( const memory::region_t& region ) const noexcept
    {
        return region.protection() != protection_flags_t::noaccess && region.state() == region_t::state_t::commit_t;
    }

    inline std::uintptr_t memory_t::checked_address( const std::uintptr_t offset, const std::size_t size ) const
    {
        if ( offset > _size || size > _size - offset )
            throw core::error::from_user(
                core::user_error_type_t::memory_out_of_range_t,
                "Memory range at offset {} with size {} is outside object size {}",
                offset,
                size,
                _size );

        if ( _address > std::numeric_limits< std::uintptr_t >::max() - offset )
            throw core::error::from_user(
                core::user_error_type_t::memory_out_of_range_t, "Memory address 0x{:X} overflows with offset {}", _address, offset );

        return _address + offset;
    }
}  // namespace wincpp::memory
