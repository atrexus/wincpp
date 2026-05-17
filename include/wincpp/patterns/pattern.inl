#pragma once

#include <algorithm>
#include <cctype>
#include <charconv>
#include <iomanip>
#include <sstream>
#include <vector>

#include "wincpp/core/error.hpp"
#include "wincpp/patterns/pattern.hpp"

namespace wincpp::patterns
{
    namespace detail
    {
        inline bool is_wildcard_token( const std::string_view token ) noexcept
        {
            return !token.empty() && std::ranges::all_of( token, []( const unsigned char ch ) { return ch == '?'; } );
        }

        inline std::optional< std::uint8_t > parse_hex_byte( const std::string_view token ) noexcept
        {
            if ( token.size() != 2 )
                return std::nullopt;

            unsigned int value{};
            const auto* begin = token.data();
            const auto* end = token.data() + token.size();
            const auto result = std::from_chars( begin, end, value, 16 );

            if ( result.ec != std::errc{} || result.ptr != end || value > 0xFF )
                return std::nullopt;

            return static_cast< std::uint8_t >( value );
        }

        inline std::optional< pattern_t > parse_ida_pattern( const std::string_view object )
        {
            std::vector< std::uint8_t > parsed_bytes;
            std::vector< bool > parsed_mask;

            for ( std::size_t offset = 0; offset < object.size(); )
            {
                while ( offset < object.size() && std::isspace( static_cast< unsigned char >( object[ offset ] ) ) )
                    ++offset;

                if ( offset == object.size() )
                    break;

                const auto token_start = offset;

                while ( offset < object.size() && !std::isspace( static_cast< unsigned char >( object[ offset ] ) ) )
                    ++offset;

                const auto token = object.substr( token_start, offset - token_start );

                if ( is_wildcard_token( token ) )
                {
                    parsed_bytes.push_back( 0 );
                    parsed_mask.push_back( false );
                    continue;
                }

                const auto byte = parse_hex_byte( token );

                if ( !byte )
                    return std::nullopt;

                parsed_bytes.push_back( *byte );
                parsed_mask.push_back( true );
            }

            pattern_t pattern;
            pattern.size = parsed_bytes.size();

            if ( pattern.size == 0 )
                return pattern;

            pattern.bytes = std::make_shared_for_overwrite< std::uint8_t[] >( pattern.size );
            pattern.mask = std::make_shared_for_overwrite< bool[] >( pattern.size );

            for ( std::size_t index = 0; index < pattern.size; ++index )
            {
                pattern.bytes[ index ] = parsed_bytes[ index ];
                pattern.mask[ index ] = parsed_mask[ index ];
            }

            return pattern;
        }
    }  // namespace detail

    inline pattern_t::pattern_t( const std::string& object ) noexcept : pattern_t( object.data(), object.size() )
    {
    }

    inline pattern_t::pattern_t( const std::string_view object ) noexcept : pattern_t( object.data(), object.size() )
    {
    }

    template< typename T >
    pattern_t::pattern_t( const T* const object, const std::size_t size ) noexcept : size( size )
    {
        bytes = std::make_shared_for_overwrite< std::uint8_t[] >( size );
        mask = std::make_shared_for_overwrite< bool[] >( size );

        const auto* raw = reinterpret_cast< const std::uint8_t* >( object );

        for ( std::size_t index = 0; index < size; ++index )
        {
            bytes[ index ] = raw[ index ];
            mask[ index ] = true;
        }
    }

    template< typename T >
    pattern_t::pattern_t( const T& object ) noexcept : pattern_t( std::addressof( object ), sizeof( T ) )
    {
    }

    inline pattern_t::pattern_t( const char* const aob, const std::string_view smask ) noexcept : size( smask.size() )
    {
        bytes = std::make_shared_for_overwrite< std::uint8_t[] >( size );
        mask = std::make_shared_for_overwrite< bool[] >( size );

        for ( std::size_t index = 0; index < size; ++index )
        {
            bytes[ index ] = static_cast< std::uint8_t >( aob[ index ] );
            mask[ index ] = smask[ index ] != '?';
        }
    }

    inline pattern_t pattern_t::from_ida( const std::string_view object )
    {
        if ( const auto pattern = try_from_ida( object ) )
            return *pattern;

        throw core::error::from_user( core::user_error_type_t::invalid_pattern_t, "Failed to parse IDA pattern \"{}\"", object );
    }

    inline std::optional< pattern_t > pattern_t::try_from_ida( const std::string_view object )
    {
        return detail::parse_ida_pattern( object );
    }

    inline std::string pattern_t::to_string() const noexcept
    {
        std::stringstream ss;

        for ( std::size_t index = 0; index < size; ++index )
        {
            if ( !mask[ index ] )
            {
                ss << "??";
                continue;
            }

            ss << std::hex << std::uppercase << std::setw( 2 ) << std::setfill( '0' ) << static_cast< int >( bytes[ index ] );
        }

        return ss.str();
    }

    inline std::ostream& operator<<( std::ostream& os, const pattern_t& pattern ) noexcept
    {
        return os << pattern.to_string();
    }
}  // namespace wincpp::patterns
