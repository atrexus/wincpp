#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include "wincpp/patterns/pattern.hpp"

namespace wincpp::patterns
{
    /// <summary>
    /// Scans a buffer of bytes for a pattern.
    /// </summary>
    class scanner final
    {
       public:
        /// <summary>
        /// The different algorithms for scanning.
        /// </summary>
        enum class algorithm_t;

        /// <summary>
        /// Searches for the pattern in the buffer.
        /// </summary>
        /// <param name="buffer">The buffer to search in.</param>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The algorithm to use.</param>
        /// <returns>The relative location.</returns>
        template< algorithm_t algorithm >
        static std::optional< std::uintptr_t > find( std::span< std::uint8_t > buffer, const pattern_t& pattern ) noexcept;

        /// <summary>
        /// Searches for all occurrences of the pattern in the buffer.
        /// </summary>
        /// <param name="buffer">The buffer to search in.</param>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The algorithm to use.</param>
        /// <returns>The relative locations.</returns>
        template< algorithm_t algorithm >
        static std::vector< std::uintptr_t > find_all( std::span< std::uint8_t > buffer, const pattern_t& pattern ) noexcept;

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
        raita_t,

        /// <summary>
        /// The Turo-BM alogithm for scanning.
        /// </summary>
        tbm_t
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
    /// The Turbo-BM algorithm for scanning.
    /// </summary>
    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::tbm_t >( const pattern_t& pattern, const std::span< std::uint8_t >& bytes ) noexcept;

    /// <summary>
    /// The Raita algorithm for scanning.
    /// </summary>
    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::raita_t >(
        const pattern_t& pattern,
        const std::span< std::uint8_t >& bytes ) noexcept;

    template< scanner::algorithm_t algorithm >
    std::optional< std::uintptr_t > scanner::find( std::span< std::uint8_t > buffer, const pattern_t& pattern ) noexcept
    {
        const auto result = scanner::index_of< algorithm >( pattern, buffer );

        if ( result != -1 )
            return result;

        return std::nullopt;
    }

    template< scanner::algorithm_t algorithm >
    std::vector< std::uintptr_t > scanner::find_all( std::span< std::uint8_t > buffer, const pattern_t& pattern ) noexcept
    {
        std::vector< std::uintptr_t > results;

        for ( auto i = 0; i < buffer.size(); )
        {
            const auto result = scanner::index_of< algorithm >( pattern, buffer.subspan( i ) );

            if ( result == -1 )
                break;

            results.push_back( i + result );
            i += result + pattern.size;
        }

        return results;
    }

}  // namespace wincpp::patterns