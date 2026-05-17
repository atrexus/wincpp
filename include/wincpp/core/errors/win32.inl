#pragma once

#include "wincpp/core/errors/win32.hpp"
#include "wincpp/detail/win32.hpp"

namespace wincpp::core
{
    inline const win32_error_category& win32_error_category::get() noexcept
    {
        static win32_error_category instance;
        return instance;
    }

    inline const char* win32_error_category::name() const noexcept
    {
        return "win32";
    }

    inline std::string win32_error_category::message( const int code ) const
    {
        char* buffer = nullptr;
        const auto size = FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            code,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            reinterpret_cast< LPSTR >( &buffer ),
            0,
            nullptr );

        if ( size == 0 )
            return "Unknown error";

        std::string message( buffer, size );
        LocalFree( buffer );

        return message;
    }
}  // namespace wincpp::core
