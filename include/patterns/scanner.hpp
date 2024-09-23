#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include "memory/region.hpp"
#include "pattern.hpp"

namespace wincpp::patterns
{
    /// <summary>
    /// Scans a buffer of bytes for a pattern.
    /// </summary>
    class scanner final
    {
        memory::memory_t object;

       public:
        /// <summary>
        /// The different algorithms for scanning.
        /// </summary>
        enum class algorithm_t;

        /// <summary>
        /// Creates a new scanner object from a memory object.
        /// </summary>
        /// <param name="object">The memory object to scan.</param>
        explicit scanner( const memory::memory_t& object );

        /// <summary>
        /// Searches for the pattern in the buffer.
        /// </summary>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The algorithm to use.</param>
        /// <returns>The relative location.</returns>
        template< algorithm_t algorithm >
        std::optional< std::uintptr_t > find( const pattern_t& pattern ) const noexcept;

        /// <summary>
        /// Searches for all occurrences of the pattern in the buffer.
        /// </summary>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The algorithm to use.</param>
        /// <returns>The relative locations.</returns>
        template< algorithm_t algorithm >
        std::vector< std::uintptr_t > find_all( const pattern_t& pattern ) const noexcept;

       private:
        /// <summary>
        /// Find the index of the pattern in the buffer.
        /// </summary>
        /// <typeparam name="algorithm">The algorithm to use for scanning.</typeparam>
        /// <param name="pattern">The pattern to scan for.</param>
        /// <param name="data">The buffer to scan.</param>
        /// <param name="size">The size of the buffer in bytes.</param>
        /// <returns>A value greater than or equal to zero if success.</returns>
        template< algorithm_t algorithm >
        static std::int64_t index_of( const pattern_t& pattern, const std::span< std::uint8_t >& span ) noexcept;

        /// <summary>
        /// Find the index of the pattern in the buffer.
        /// </summary>
        /// <typeparam name="algorithm">The algorithm to use for scanning.</typeparam>
        /// <param name="pattern">The pattern to scan for.</param>
        /// <param name="begin">The beginning of the buffer.</param>
        /// <param name="end">The end of the buffer.</param>
        template< algorithm_t algorithm >
        static std::int64_t index_of( const pattern_t& pattern, std::uint8_t* begin, std::uint8_t* end ) noexcept;
    };

    enum class scanner::algorithm_t
    {
        /// <summary>
        /// The naive algorithm for scanning.
        /// </summary>
        naive_t,

        /// <summary>
        /// The Boyer-Moore-Horspool algorithm for scanning.
        /// </summary>
        bmh_t,

        /// <summary>
        /// The Raita algorithm for scanning.
        /// </summary>
        raita_t
    };

    /// <summary>
    /// The naive algorithm for scanning.
    /// </summary>
    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::naive_t >(
        const pattern_t& pattern,
        const std::span< std::uint8_t >& bytes ) noexcept;

    /// <summary>
    /// The Boyer-Moore-Horspool algorithm for scanning.
    /// </summary>
    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::bmh_t >( const pattern_t& pattern, const std::span< std::uint8_t >& bytes ) noexcept;

    /// <summary>
    /// The Raita algorithm for scanning.
    /// </summary>
    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::raita_t >(
        const pattern_t& pattern,
        const std::span< std::uint8_t >& bytes ) noexcept;

    template< scanner::algorithm_t algorithm >
    inline static std::int64_t scanner::index_of( const pattern_t& pattern, std::uint8_t* begin, std::uint8_t* end ) noexcept
    {
        return scanner::index_of< algorithm >( pattern, std::span< std::uint8_t >( begin, end - begin ) );
    }

    template< scanner::algorithm_t algorithm >
    std::optional< std::uintptr_t > scanner::find( const pattern_t& pattern ) const noexcept
    {
        for ( const auto& region : object.regions() )
        {
            if ( region.address() > object.address() + object.size() )
                break;

            if ( region.state() != memory::region_t::state_t::commit_t || region.protection().has( memory::protection_t::noaccess_t ) )
                continue;

            const auto buffer = region.read( region.address(), region.size() );

            const auto result = scanner::index_of< algorithm >( pattern, { buffer.get(), region.size() } );

            if ( result != -1 )
                return region.address() + result;
        }

        return std::nullopt;
    }

    template< scanner::algorithm_t algorithm >
    std::vector< std::uintptr_t > scanner::find_all( const pattern_t& pattern ) const noexcept
    {
        std::vector< std::uintptr_t > results;

        for ( const auto& region : object.regions() )
        {
            if ( region.address() > object.address() + object.size() )
                break;

            if ( region.state() != memory::region_t::state_t::commit_t || region.protection().has( memory::protection_t::noaccess_t ) )
                continue;

            const auto buffer = region.read( region.address(), region.size() );

            const auto begin = buffer.get();
            const auto end = begin + region.size();

            for ( auto it = begin; it != end; )
            {
                const auto index = scanner::index_of< algorithm >( pattern, it, end );

                if ( index == -1 )
                    break;

                results.push_back( region.address() + ( it - begin ) + index );
                it += index + 1;
            }
        }

        return results;
    }

}  // namespace wincpp::patterns