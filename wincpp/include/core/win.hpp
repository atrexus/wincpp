#pragma once

// clang-format off
#include <Windows.h>
// clang-format on

#include <expected>
#include <memory>
#include <string>

namespace wincpp::core
{
    /// <summary>
    /// A safe handle to a Windows object.
    /// </summary>
    struct handle_t
    {
        /// <summary>
        /// A custom deleter object for handle pointers.
        /// </summary>
        struct deleter
        {
            /// <summary>
            /// The actual deleter method (deletes pointer & closes handle).
            /// </summary>
            /// <param name="handle"></param>
            void operator()( handle_t* handle ) const;
        };

        /// <summary>
        /// Creates a safe handle from a native handle.
        /// </summary>
        /// <param name="hHandle"></param>
        /// <returns></returns>
        static std::shared_ptr< handle_t > create( HANDLE hHandle, bool owns_handle = true );

        /// <summary>
        /// The native handle.
        /// </summary>
        HANDLE native;

       private:
        /// <summary>
        /// Creates a new handle object.
        /// </summary>
        handle_t( HANDLE hHandle, bool owns_handle );

        /// <summary>
        /// Whether the handle object owns the handle.
        /// </summary>
        bool owns_handle;
    };
}  // namespace wincpp::core