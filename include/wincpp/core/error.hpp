#pragma once

#include <expected>
#include <format>
#include <system_error>
#include <utility>

#include "wincpp/core/errors/user.hpp"
#include "wincpp/core/errors/win32.hpp"

namespace wincpp::core
{
    /// <summary>
    /// Represents an error returned by the Windows API.
    /// </summary>
    class error : public std::system_error
    {
        using std::system_error::system_error;

       public:
        /// <summary>
        /// Creates a new error object with the given error code.
        /// </summary>
        /// <param name="code">The Win32 error code.</param>
        /// <returns>The error object.</returns>
        static error from_win32( std::uint32_t code ) noexcept;

        /// <summary>
        /// Creates a new error object with the given user-defined error and formatted message.
        /// </summary>
        /// <typeparam name="T">The formatting argument types.</typeparam>
        /// <param name="code">The user-defined error code.</param>
        /// <param name="format">The format string.</param>
        /// <param name="args">The format arguments.</param>
        /// <returns>The error object.</returns>
        template< typename... T >
        static error from_user( user_error_type_t code, const std::format_string< T... > format, T&&... args ) noexcept;
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

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/core/error.inl"
#endif
