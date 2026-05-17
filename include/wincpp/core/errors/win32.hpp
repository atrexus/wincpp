#pragma once

#include <string>
#include <system_error>

namespace wincpp::core
{
    /// <summary>
    /// The error category for Win32 errors.
    /// </summary>
    struct win32_error_category : public std::error_category
    {
        /// <summary>
        /// Returns the error category for Win32 errors.
        /// </summary>
        /// <returns>The Win32 error category.</returns>
        static const win32_error_category& get() noexcept;

        /// <summary>
        /// Returns the name of the error category.
        /// </summary>
        /// <returns>The category name.</returns>
        const char* name() const noexcept override;

        /// <summary>
        /// Returns the error message for the given error code.
        /// </summary>
        /// <param name="code">The error code.</param>
        /// <returns>The error message.</returns>
        std::string message( int code ) const override;
    };
}  // namespace wincpp::core

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/core/errors/win32.inl"
#endif
