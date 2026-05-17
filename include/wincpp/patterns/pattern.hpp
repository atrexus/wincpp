#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>

namespace wincpp::patterns
{
    /// <summary>
    /// The class for all patterns. This struct contains the bytes, mask, and size of the pattern.
    /// </summary>
    struct pattern_t
    {
        /// <summary>
        /// Default constructor for the pattern object.
        /// </summary>
        pattern_t() = default;

        /// <summary>
        /// Creates a new pattern object with the specified object. This function will attempt to convert it to an array of bytes.
        /// </summary>
        /// <typeparam name="T">The type of the object.</typeparam>
        /// <param name="object">The object.</param>
        template< typename T >
        pattern_t( const T& object ) noexcept;

        /// <summary>
        /// Creates a new pattern from the bytes of the string (each character is a byte).
        /// </summary>
        /// <param name="object">The string.</param>
        pattern_t( const std::string& object ) noexcept;

        /// <summary>
        /// Creates a new pattern from the bytes of the string (each character is a byte).
        /// </summary>
        /// <param name="object">The string.</param>
        pattern_t( std::string_view object ) noexcept;

        /// <summary>
        /// Creates a new pattern object with the specified pointer and its size.
        /// </summary>
        /// <typeparam name="T">The type of the pointer.</typeparam>
        /// <param name="object">The object pointer.</param>
        /// <param name="size">The object size.</param>
        template< typename T >
        pattern_t( const T* object, std::size_t size ) noexcept;

        /// <summary>
        /// Creates a new pattern object with the specified array of bytes and mask. This is an IDA-style pattern.
        /// Example: "\xA1\x00\x00\x00\x00\xB2", "x????x"
        /// </summary>
        /// <param name="aob">The byte sequence.</param>
        /// <param name="smask">The mask sequence.</param>
        pattern_t( const char* aob, std::string_view smask ) noexcept;

        /// <summary>
        /// Creates a pattern from an IDA-style pattern string.
        /// Example: "48 8D 0D ? ? ? ? 48 8D".
        /// </summary>
        /// <param name="object">The IDA-style pattern string.</param>
        /// <returns>The parsed pattern.</returns>
        static pattern_t from_ida( std::string_view object );

        /// <summary>
        /// Attempts to create a pattern from an IDA-style pattern string without throwing an exception for invalid input.
        /// Example: "48 8D 0D ? ? ? ? 48 8D".
        /// </summary>
        /// <param name="object">The IDA-style pattern string.</param>
        /// <returns>The parsed pattern, if parsing succeeded.</returns>
        static std::optional< pattern_t > try_from_ida( std::string_view object );

        /// <summary>
        /// Converts the pattern to a string.
        /// </summary>
        /// <returns>The string representation.</returns>
        std::string to_string() const noexcept;

        /// <summary>
        /// Writes the current pattern to the specified output stream.
        /// </summary>
        /// <param name="os">The output stream.</param>
        /// <param name="pattern">The pattern object.</param>
        /// <returns>The output stream.</returns>
        friend std::ostream& operator<<( std::ostream& os, const pattern_t& pattern ) noexcept;

        /// <summary>
        /// The pattern bytes.
        /// </summary>
        std::shared_ptr< std::uint8_t[] > bytes;

        /// <summary>
        /// The byte mask.
        /// </summary>
        std::shared_ptr< bool[] > mask;

        /// <summary>
        /// The pattern size.
        /// </summary>
        std::size_t size = 0;
    };
}  // namespace wincpp::patterns

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/patterns/pattern.inl"
#endif
