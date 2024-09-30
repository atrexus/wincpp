#pragma once

#include <memory>
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
        template<>
        pattern_t( const std::string& object ) noexcept : pattern_t( object.data(), object.size() )
        {
        }

        /// <summary>
        /// Creates a new pattern from the bytes of the string (each character is a byte).
        /// </summary>
        /// <param name="object">The string.</param>
        template<>
        pattern_t( const std::string_view& object ) noexcept : pattern_t( object.data(), object.size() )
        {
        }

        /// <summary>
        /// Creates a new pattern object with the specified pointer and its size.
        /// </summary>
        /// <typeparam name="T">The type of the pointer.</typeparam>
        /// <param name="object"></param>
        /// <param name="size"></param>
        template< typename T >
        pattern_t( const T* object, std::size_t size ) noexcept;

        /// <summary>
        /// Creates a new pattern object with the specified array of bytes and mask. This is an IDA-style pattern.
        /// Example: "\xA1\x00\x00\x00\x00\xB2", "x????x"
        /// </summary>
        /// <param name="aob"></param>
        /// <param name="mask"></param>
        pattern_t( const char* const aob, const std::string_view smask ) noexcept;

        /// <summary>
        /// Converts the pattern to a string.
        /// </summary>
        std::string to_string() const noexcept;

        /// <summary>
        /// Writes the current pattern to the specified output stream.
        /// </summary>
        friend std::ostream& operator<<( std::ostream& os, const pattern_t& p ) noexcept;

        std::shared_ptr< std::uint8_t[] > bytes;
        std::shared_ptr< bool[] > mask;
        std::size_t size = 0;
    };

    template< typename T >
    inline pattern_t::pattern_t( const T* object, std::size_t size ) noexcept : size( size )
    {
        bytes = std::shared_ptr< std::uint8_t[] >( new std::uint8_t[ size ] );
        mask = std::shared_ptr< bool[] >( new bool[ size ] );

        for ( std::size_t i = 0; i < size; ++i )
        {
            bytes[ i ] = reinterpret_cast< const std::uint8_t* >( object )[ i ];
            mask[ i ] = true;
        }
    }

    template< typename T >
    pattern_t::pattern_t( const T& object ) noexcept : pattern_t( std::addressof( object ), sizeof( T ) )
    {
    }

}  // namespace wincpp::patterns