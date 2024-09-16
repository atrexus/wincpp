#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace wincpp::patterns
{
    /// <summary>
    /// The base class for all patterns. This struct contains the bytes, mask, and size of the pattern.
    /// </summary>
    struct pattern_t
    {
        /// <summary>
        /// Cretes a new pattern object with the specified pattern. The pattern is converted to bytes and a mask. Example: "A1 ? ? ? ? B2"
        /// </summary>
        /// <param name="pattern">The pattern of bytes.</param>
        pattern_t( const std::string_view pattern ) noexcept;

       protected:
        std::shared_ptr< std::uint8_t[] > bytes;
        std::shared_ptr< bool[] > mask;
        std::size_t size;
    };
}  // namespace wincpp::patterns