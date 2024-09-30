#include "wincpp/patterns/pattern.hpp"

#include <sstream>
#include <string_view>
#include <vector>

namespace wincpp::patterns
{
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