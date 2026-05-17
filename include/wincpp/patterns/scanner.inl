#pragma once

#include <algorithm>
#include <array>
#include <ranges>

#include "wincpp/patterns/scanner.hpp"

namespace wincpp::patterns
{
    namespace detail
    {
        inline bool matches_at( const pattern_t& pattern, const std::span< const std::uint8_t > bytes, const std::size_t offset ) noexcept
        {
            if ( offset + pattern.size > bytes.size() )
                return false;

            for ( std::size_t index = 0; index < pattern.size; ++index )
            {
                if ( pattern.mask[ index ] && pattern.bytes[ index ] != bytes[ offset + index ] )
                    return false;
            }

            return true;
        }

        inline std::array< std::size_t, 256 > make_skip_table( const pattern_t& pattern ) noexcept
        {
            std::array< std::size_t, 256 > skip_table{};
            skip_table.fill( pattern.size );

            if ( pattern.size == 0 )
                return skip_table;

            for ( const auto index : std::views::iota( std::size_t{ 0 }, pattern.size - 1 ) )
            {
                if ( pattern.mask[ index ] )
                    skip_table[ pattern.bytes[ index ] ] = pattern.size - 1 - index;
            }

            return skip_table;
        }
    }  // namespace detail

    template<>
    inline std::optional< std::size_t > scanner::index_of< scanner::algorithm_t::naive_t >(
        const pattern_t& pattern,
        const std::span< const std::uint8_t > bytes ) noexcept
    {
        if ( pattern.size == 0 || bytes.empty() || pattern.size > bytes.size() )
            return std::nullopt;

        for ( const auto offset : std::views::iota( std::size_t{ 0 }, bytes.size() - pattern.size + 1 ) )
        {
            if ( detail::matches_at( pattern, bytes, offset ) )
                return offset;
        }

        return std::nullopt;
    }

    template<>
    inline std::optional< std::size_t > scanner::index_of< scanner::algorithm_t::bmh_t >(
        const pattern_t& pattern,
        const std::span< const std::uint8_t > bytes ) noexcept
    {
        if ( pattern.size == 0 || bytes.empty() || pattern.size > bytes.size() )
            return std::nullopt;

        const auto skip_table = detail::make_skip_table( pattern );
        std::size_t offset = 0;

        while ( offset <= bytes.size() - pattern.size )
        {
            auto matched = true;

            for ( auto pattern_index = pattern.size; pattern_index-- > 0; )
            {
                if ( pattern.mask[ pattern_index ] && pattern.bytes[ pattern_index ] != bytes[ offset + pattern_index ] )
                {
                    matched = false;
                    break;
                }
            }

            if ( matched )
                return offset;

            offset += skip_table[ bytes[ offset + pattern.size - 1 ] ];
        }

        return std::nullopt;
    }

    template<>
    inline std::optional< std::size_t > scanner::index_of< scanner::algorithm_t::tbm_t >(
        const pattern_t& pattern,
        const std::span< const std::uint8_t > bytes ) noexcept
    {
        if ( pattern.size == 0 || bytes.empty() || pattern.size > bytes.size() )
            return std::nullopt;

        const auto skip_table = detail::make_skip_table( pattern );
        std::size_t turbo_shift = 0;
        std::size_t shift = 0;
        std::size_t offset = 0;

        while ( offset <= bytes.size() - pattern.size )
        {
            std::optional< std::size_t > mismatch_index;

            for ( auto pattern_index = pattern.size; pattern_index-- > 0; )
            {
                if ( pattern.mask[ pattern_index ] && pattern.bytes[ pattern_index ] != bytes[ offset + pattern_index ] )
                {
                    mismatch_index = pattern_index;
                    break;
                }
            }

            if ( !mismatch_index )
                return offset;

            const auto last_byte = bytes[ offset + pattern.size - 1 ];

            if ( turbo_shift > 0 )
            {
                shift = std::max( std::size_t{ 1 }, skip_table[ last_byte ] );
                turbo_shift = 0;
            }
            else
            {
                shift = skip_table[ last_byte ];

                if ( *mismatch_index < pattern.size - 1 )
                    turbo_shift = pattern.size - 1 - *mismatch_index;
            }

            offset += std::max( shift, turbo_shift );
        }

        return std::nullopt;
    }

    template<>
    inline std::optional< std::size_t > scanner::index_of< scanner::algorithm_t::raita_t >(
        const pattern_t& pattern,
        const std::span< const std::uint8_t > bytes ) noexcept
    {
        if ( pattern.size == 0 || bytes.empty() || pattern.size > bytes.size() )
            return std::nullopt;

        const auto last_index = pattern.size - 1;
        const auto middle_index = pattern.size / 2;
        const auto skip_table = detail::make_skip_table( pattern );
        std::size_t offset = 0;

        while ( offset <= bytes.size() - pattern.size )
        {
            if ( ( !pattern.mask[ last_index ] || pattern.bytes[ last_index ] == bytes[ offset + last_index ] ) &&
                 ( !pattern.mask[ 0 ] || pattern.bytes[ 0 ] == bytes[ offset ] ) &&
                 ( !pattern.mask[ middle_index ] || pattern.bytes[ middle_index ] == bytes[ offset + middle_index ] ) )
                if ( detail::matches_at( pattern, bytes, offset ) )
                    return offset;

            offset += skip_table[ bytes[ offset + last_index ] ];
        }

        return std::nullopt;
    }

    template< scanner::algorithm_t algorithm >
    std::optional< std::uintptr_t > scanner::find( const std::span< const std::uint8_t > buffer, const pattern_t& pattern ) noexcept
    {
        const auto result = scanner::index_of< algorithm >( pattern, buffer );

        if ( result )
            return *result;

        return std::nullopt;
    }

    inline std::optional< std::uintptr_t >
    scanner::find( const std::span< const std::uint8_t > buffer, const pattern_t& pattern, const algorithm_t algorithm ) noexcept
    {
        switch ( algorithm )
        {
            case algorithm_t::naive_t: return find< algorithm_t::naive_t >( buffer, pattern );
            case algorithm_t::bmh_t: return find< algorithm_t::bmh_t >( buffer, pattern );
            case algorithm_t::raita_t: return find< algorithm_t::raita_t >( buffer, pattern );
            case algorithm_t::tbm_t: return find< algorithm_t::tbm_t >( buffer, pattern );
        }

        return std::nullopt;
    }

    template< scanner::algorithm_t algorithm >
    std::vector< std::uintptr_t > scanner::find_all( const std::span< const std::uint8_t > buffer, const pattern_t& pattern ) noexcept
    {
        std::vector< std::uintptr_t > results;

        for ( std::size_t offset = 0; offset < buffer.size(); )
        {
            const auto result = scanner::index_of< algorithm >( pattern, buffer.subspan( offset ) );

            if ( !result )
                break;

            results.push_back( offset + *result );
            offset += *result + 1;
        }

        return results;
    }

    inline std::vector< std::uintptr_t >
    scanner::find_all( const std::span< const std::uint8_t > buffer, const pattern_t& pattern, const algorithm_t algorithm ) noexcept
    {
        switch ( algorithm )
        {
            case algorithm_t::naive_t: return find_all< algorithm_t::naive_t >( buffer, pattern );
            case algorithm_t::bmh_t: return find_all< algorithm_t::bmh_t >( buffer, pattern );
            case algorithm_t::raita_t: return find_all< algorithm_t::raita_t >( buffer, pattern );
            case algorithm_t::tbm_t: return find_all< algorithm_t::tbm_t >( buffer, pattern );
        }

        return {};
    }
}  // namespace wincpp::patterns
