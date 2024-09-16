#include "patterns/scanner.hpp"

namespace wincpp::patterns
{
    scanner::scanner( std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) noexcept : buffer( buffer ), size( size )
    {
    }
}  // namespace wincpp::patterns