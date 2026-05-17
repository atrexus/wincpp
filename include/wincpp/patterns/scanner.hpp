#pragma once

#include <cstdint>
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
        enum class algorithm_t : std::uint8_t
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
            /// The Turbo-BM algorithm for scanning.
            /// </summary>
            tbm_t
        };

        /// <summary>
        /// Searches for the pattern in the buffer.
        /// </summary>
        /// <typeparam name="algorithm">The algorithm to use.</typeparam>
        /// <param name="buffer">The buffer to search in.</param>
        /// <param name="pattern">The pattern to search for.</param>
        /// <returns>The relative location.</returns>
        template< algorithm_t algorithm >
        static std::optional< std::uintptr_t > find( std::span< const std::uint8_t > buffer, const pattern_t& pattern ) noexcept;

        /// <summary>
        /// Searches for the pattern in the buffer.
        /// </summary>
        /// <param name="buffer">The buffer to search in.</param>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The algorithm to use.</param>
        /// <returns>The relative location.</returns>
        static std::optional< std::uintptr_t >
        find( std::span< const std::uint8_t > buffer, const pattern_t& pattern, algorithm_t algorithm ) noexcept;

        /// <summary>
        /// Searches for all occurrences of the pattern in the buffer.
        /// </summary>
        /// <typeparam name="algorithm">The algorithm to use.</typeparam>
        /// <param name="buffer">The buffer to search in.</param>
        /// <param name="pattern">The pattern to search for.</param>
        /// <returns>The relative locations.</returns>
        template< algorithm_t algorithm >
        static std::vector< std::uintptr_t > find_all( std::span< const std::uint8_t > buffer, const pattern_t& pattern ) noexcept;

        /// <summary>
        /// Searches for all occurrences of the pattern in the buffer.
        /// </summary>
        /// <param name="buffer">The buffer to search in.</param>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The algorithm to use.</param>
        /// <returns>The relative locations.</returns>
        static std::vector< std::uintptr_t >
        find_all( std::span< const std::uint8_t > buffer, const pattern_t& pattern, algorithm_t algorithm ) noexcept;

       private:
        /// <summary>
        /// Find the index of the pattern in the buffer.
        /// </summary>
        /// <typeparam name="algorithm">The algorithm to use for scanning.</typeparam>
        /// <param name="pattern">The pattern to scan for.</param>
        /// <param name="bytes">The buffer to scan.</param>
        /// <returns>A value greater than or equal to zero if success.</returns>
        template< algorithm_t algorithm >
        static std::optional< std::size_t > index_of( const pattern_t& pattern, std::span< const std::uint8_t > bytes ) noexcept;
    };
}  // namespace wincpp::patterns

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/patterns/scanner.inl"
#endif
