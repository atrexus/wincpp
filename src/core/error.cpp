#include "core/error.hpp"

namespace wincpp::core
{
    error::error( std::error_code code ) noexcept : std::system_error( code )
    {
    }

    error error::from_win32( std::uint32_t code ) noexcept
    {
        return error( std::error_code( code, win32_error_category::get() ) );
    }
}  // namespace wincpp::core
