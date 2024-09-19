#include "patterns/pattern.hpp"

#include <sstream>
#include <string_view>
#include <vector>

namespace wincpp::patterns
{
    static std::vector< std::string_view > split_string( const std::string_view str )
    {
        std::vector< std::string_view > result;
        std::size_t start = 0, end;

        while ( ( end = str.find( ' ', start ) ) != std::string_view::npos )
        {
            if ( end != start )
            {  // Skip consecutive spaces
                result.emplace_back( str.substr( start, end - start ) );
            }
            start = end + 1;
        }

        if ( start < str.size() )
        {  // Add the last word if there's any
            result.emplace_back( str.substr( start ) );
        }

        return result;
    }

    pattern_t::pattern_t( const std::string_view pattern ) noexcept : size( 0 )
    {
        const auto split = split_string( pattern );

        size = split.size();

        // Allocate memory for our bytes and mask.
        bytes = std::shared_ptr< std::uint8_t[] >( new std::uint8_t[ size ] );
        mask = std::shared_ptr< bool[] >( new bool[ size ] );

        // Iterate over the split pattern and update the bytes and mask.
        for ( std::size_t i = 0; i < size; ++i )
        {
            const auto& byte = split[ i ];

            if ( byte == "?" )
            {
                mask[ i ] = false;
                bytes[ i ] = 0;
            }
            else
            {
                mask[ i ] = true;
                bytes[ i ] = static_cast< std::uint8_t >( std::stoi( byte.data(), nullptr, 16 ) );
            }
        }
    }

    pattern_t::pattern_t( const char* const aob, const std::string_view smask ) noexcept : size( smask.size() )
    {
        bytes = std::shared_ptr< std::uint8_t[] >( new std::uint8_t[ size ] );
        mask = std::shared_ptr< bool[] >( new bool[ size ] );

        for ( std::size_t i = 0; i < size; ++i )
        {
            bytes[ i ] = aob[ i ];
            mask[ i ] = smask[ i ] == 'x';
        }
    }

    pattern_t pattern_t::from( const char* const s ) noexcept
    {
        return from( s, std::strlen( s ) );
    }

    pattern_t pattern_t::from( const char* const s, std::size_t size ) noexcept
    {
        pattern_t p{};

        p.size = size;
        p.bytes = std::make_shared< std::uint8_t[] >( size );
        p.mask = std::make_shared< bool[] >( size );

        for ( std::size_t i = 0; i < size; ++i )
        {
            p.bytes[ i ] = s[ i ];
            p.mask[ i ] = true;
        }

        return p;
    }

    std::string pattern_t::to_string() const noexcept
    {
        std::stringstream ss;

        for ( std::size_t i = 0; i < size; ++i )
        {
            if ( mask[ i ] )
            {
                ss << std::hex << static_cast< int >( bytes[ i ] );
            }
            else
            {
                ss << "?";
            }

            if ( i + 1 < size )
            {
                ss << " ";
            }
        }

        return ss.str();
    }

    std::ostream& operator<<( std::ostream& os, const pattern_t& p ) noexcept
    {
        return os << p.to_string();
    }
}  // namespace wincpp::patterns