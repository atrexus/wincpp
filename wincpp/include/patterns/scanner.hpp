#pragma once

#include <memory>

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
        /// Creates a new scanner object.
        /// </summary>
        /// <param name="buffer">The source buffer to search.</param>
        /// <param name="size">The size of the buffer in bytes.</param>
        explicit scanner( std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) noexcept;
    };
}  // namespace wincpp::patterns