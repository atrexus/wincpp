#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "pattern.hpp"

namespace wincpp::patterns
{
    /// <summary>
    /// Scans a buffer of bytes for a pattern.
    /// </summary>
    class scanner final
    {
        std::shared_ptr< std::uint8_t[] > buffer;
        std::size_t size;

       public:
        /// <summary>
        /// The different algorithms for scanning.
        /// </summary>
        enum class algorithm_t;

        /// <summary>
        /// Creates a new scanner object.
        /// </summary>
        /// <param name="buffer">The source buffer to search.</param>
        /// <param name="size">The size of the buffer in bytes.</param>
        explicit scanner( std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) noexcept;

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
        static std::int64_t index_of( const pattern_t& pattern, std::uint8_t* data, std::size_t size ) noexcept;
    };

    enum class scanner::algorithm_t
    {
        /// <summary>
        /// The naive algorithm for scanning.
        /// </summary>
        naive_t,

        /// <summary>
        /// The Knuth-Morris-Pratt algorithm for scanning.
        /// </summary>
        kmp_t,

        /// <summary>
        /// The Boyer-Moore-Horspool algorithm for scanning.
        /// </summary>
        bmh_t,
    };

    /// <summary>
    /// The naive algorithm for scanning.
    /// </summary>
    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::naive_t >( const pattern_t& pattern, std::uint8_t* data, std::size_t size ) noexcept;

    /// <summary>
    /// The Knuth-Morris-Pratt algorithm for scanning.
    /// </summary>
    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::kmp_t >( const pattern_t& pattern, std::uint8_t* data, std::size_t size ) noexcept;

    /// <summary>
    /// The Boyer-Moore-Horspool algorithm for scanning.
    /// </summary>
    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::bmh_t >( const pattern_t& pattern, std::uint8_t* data, std::size_t size ) noexcept;

}  // namespace wincpp::patterns