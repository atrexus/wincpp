#pragma once

#include <expected>
#include <system_error>

#include "core/errors/win32.hpp"

namespace wincpp::core
{
    /// <summary>
    /// Represents an error returned by the Windows API.
    /// </summary>
    class error : public std::system_error
    {
        /// <summary>
        /// Creates a new error object with the given error code.
        /// </summary>
        error( std::error_code code ) noexcept;

       public:
        /// <summary>
        /// Creates a new error object with the given error code.
        /// </summary>
        static error from_win32( std::uint32_t code ) noexcept;
    };

    /// <summary>
    /// The result type for the Windows API.
    /// </summary>
    template< typename T >
    using result_t = std::expected< T, error >;

    /// <summary>
    /// The unexpected type for the Windows API.
    /// </summary>
    using unexpected_t = std::unexpected< error >;

}  // namespace wincpp::core