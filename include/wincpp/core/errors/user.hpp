#pragma once

#include <cstdint>
#include <string>
#include <system_error>

namespace wincpp::core
{
    /// <summary>
    /// The different types of user-defined errors.
    /// </summary>
    enum class user_error_type_t : std::uint8_t
    {
        /// <summary>
        /// The desired module was not found.
        /// </summary>
        module_not_found_t,

        /// <summary>
        /// The desired thread was not found.
        /// </summary>
        thread_not_found_t,

        /// <summary>
        /// The desired process was not found.
        /// </summary>
        process_not_found_t,

        /// <summary>
        /// The desired export was not found.
        /// </summary>
        export_not_found_t,

        /// <summary>
        /// The requested memory range was outside the bounds of the memory object.
        /// </summary>
        memory_out_of_range_t,

        /// <summary>
        /// The requested pattern string could not be parsed.
        /// </summary>
        invalid_pattern_t,

        /// <summary>
        /// The operation failed before a more specific error could be reported.
        /// </summary>
        operation_failed_t
    };

    /// <summary>
    /// The error category for user-defined errors (errors that wincpp creates).
    /// </summary>
    struct user_error_category : public std::error_category
    {
        /// <summary>
        /// Returns the error category for user errors.
        /// </summary>
        /// <returns>The user error category.</returns>
        static const user_error_category& get() noexcept;

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
#include "wincpp/core/errors/user.inl"
#endif
