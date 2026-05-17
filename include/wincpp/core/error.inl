#pragma once

#include "wincpp/core/error.hpp"

namespace wincpp::core
{
    inline error error::from_win32( const std::uint32_t code ) noexcept
    {
        return { std::error_code( static_cast< int >( code ), win32_error_category::get() ) };
    }

    template< typename... T >
    error error::from_user( const user_error_type_t code, const std::format_string< T... > format, T&&... args ) noexcept
    {
        const auto user_error_code = std::error_code( std::to_underlying( code ), user_error_category::get() );

        try
        {
            return { user_error_code, std::format( format, std::forward< T >( args )... ) };
        }
        catch ( const std::exception& )
        {
            return { user_error_code };
        }
    }
}  // namespace wincpp::core
