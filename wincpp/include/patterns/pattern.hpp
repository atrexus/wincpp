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
        /// Creates a new pattern object with the specified pattern. The pattern is converted to bytes and a mask.
        /// Example: "A1 ? ? ? ? B2"
        /// </summary>
        /// <param name="pattern">The pattern of bytes.</param>
        pattern_t( const std::string_view pattern ) noexcept;

        /// <summary>
        /// Creates a new pattern object with the specified array of bytes and mask. This is an IDA-style pattern.
        /// Example: "\xA1\x00\x00\x00\x00\xB2", "x????x"
        /// </summary>
        /// <param name="aob"></param>
        /// <param name="mask"></param>
        pattern_t( const char* const aob, const std::string_view smask ) noexcept;

        /// <summary>
        /// Creates a new pattern object from the specified object. This function will attempt to convert it to an array of bytes.
        /// </summary>
        template< typename T >
        static pattern_t from( const T& object ) noexcept;

        /// <summary>
        /// Creates a new pattern object from the specified string.
        /// </summary>
        static pattern_t from( const char* const s ) noexcept;

        /// <summary>
        /// Creates a new pattern object from the specified string.
        /// </summary>
        /// <param name="s">The string to convert to a pattern.</param>
        /// <param name="size">The size of the string.</param>
        static pattern_t from( const char* const s, std::size_t size ) noexcept;

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

    template<>
    inline pattern_t pattern_t::from< std::string >( const std::string& object ) noexcept
    {
        return from( object.c_str(), object.size() );
    }

    template<>
    inline pattern_t pattern_t::from< std::string_view >( const std::string_view& object ) noexcept
    {
        return from( object.data(), object.size() );
    }

    template< typename T >
    inline pattern_t pattern_t::from( const T& object ) noexcept
    {
        pattern_t p{};

        p.size = sizeof( T );
        p.bytes = std::make_shared< std::uint8_t[] >( sizeof( T ) );
        p.mask = std::make_shared< bool[] >( sizeof( T ) );

        const auto begin = reinterpret_cast< const std::uint8_t* >( std::addressof( object ) );

        for ( auto i = 0; i < p.size; ++i )
        {
            p.bytes[ i ] = begin[ i ];
            p.mask[ i ] = true;
        }

        // print the pattern
        for (int i = 0; i < p.size; ++i)
        {
            printf( "%02X ", p.bytes[ i ] );
        }

        return p;
    }

}  // namespace wincpp::patterns