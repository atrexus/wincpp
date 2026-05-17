#pragma once

#include <cstdint>
#include <memory>
#include <utility>

namespace wincpp::core
{
    /// <summary>
    /// Opaque native handle value used for Windows handles without exposing Windows SDK declarations.
    /// </summary>
    using native_handle_t = void*;

    /// <summary>
    /// Opaque native handle value used for Windows window handles without exposing Windows SDK declarations.
    /// </summary>
    using native_window_handle_t = void*;

    /// <summary>
    /// Process access rights understood by process_t::open.
    /// </summary>
    enum class process_access_t : std::uint32_t
    {
        /// <summary>
        /// No process access rights.
        /// </summary>
        none_t = 0,

        /// <summary>
        /// Required to terminate a process.
        /// </summary>
        terminate_t = 0x00000001,

        /// <summary>
        /// Required to create a thread in a process.
        /// </summary>
        create_thread_t = 0x00000002,

        /// <summary>
        /// Required to perform an operation on the address space of a process.
        /// </summary>
        vm_operation_t = 0x00000008,

        /// <summary>
        /// Required to read memory in a process.
        /// </summary>
        vm_read_t = 0x00000010,

        /// <summary>
        /// Required to write memory in a process.
        /// </summary>
        vm_write_t = 0x00000020,

        /// <summary>
        /// Required to duplicate a process handle.
        /// </summary>
        duplicate_handle_t = 0x00000040,

        /// <summary>
        /// Required to create a process.
        /// </summary>
        create_process_t = 0x00000080,

        /// <summary>
        /// Required to set memory limits using SetProcessWorkingSetSize.
        /// </summary>
        set_quota_t = 0x00000100,

        /// <summary>
        /// Required to set certain information about a process.
        /// </summary>
        set_information_t = 0x00000200,

        /// <summary>
        /// Required to retrieve certain information about a process.
        /// </summary>
        query_information_t = 0x00000400,

        /// <summary>
        /// Required to suspend or resume a process.
        /// </summary>
        suspend_resume_t = 0x00000800,

        /// <summary>
        /// Required to retrieve limited information about a process.
        /// </summary>
        query_limited_information_t = 0x00001000,

        /// <summary>
        /// Required to wait for the process to terminate.
        /// </summary>
        synchronize_t = 0x00100000,

        /// <summary>
        /// All access rights available for a process on supported Windows versions.
        /// </summary>
        all_t = 0x001FFFFF
    };

    /// <summary>
    /// Combines two process access flag sets.
    /// </summary>
    /// <param name="lhs">The left-hand side access flag set.</param>
    /// <param name="rhs">The right-hand side access flag set.</param>
    /// <returns>The combined process access flag set.</returns>
    constexpr process_access_t operator|( process_access_t lhs, process_access_t rhs ) noexcept;

    /// <summary>
    /// Intersects two process access flag sets.
    /// </summary>
    /// <param name="lhs">The left-hand side access flag set.</param>
    /// <param name="rhs">The right-hand side access flag set.</param>
    /// <returns>The intersected process access flag set.</returns>
    constexpr process_access_t operator&( process_access_t lhs, process_access_t rhs ) noexcept;

    /// <summary>
    /// Adds process access flags to an existing flag set.
    /// </summary>
    /// <param name="lhs">The access flag set to modify.</param>
    /// <param name="rhs">The access flags to add.</param>
    /// <returns>The modified process access flag set.</returns>
    constexpr process_access_t& operator|=( process_access_t& lhs, process_access_t rhs ) noexcept;

    /// <summary>
    /// Determines whether a process access flag set contains another access flag set.
    /// </summary>
    /// <param name="value">The access flag set to inspect.</param>
    /// <param name="flags">The access flags to check for.</param>
    /// <returns>True if all requested flags are present.</returns>
    constexpr bool has_flag( process_access_t value, process_access_t flags ) noexcept;

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
            /// The actual deleter method (deletes pointer and closes handle).
            /// </summary>
            /// <param name="handle">The handle wrapper to delete.</param>
            void operator()( handle_t* handle ) const;
        };

        /// <summary>
        /// Creates a safe handle from a native handle.
        /// </summary>
        /// <param name="handle">The native handle.</param>
        /// <param name="owns_handle">Whether this object owns the handle.</param>
        /// <returns>The safe handle.</returns>
        static std::shared_ptr< handle_t > create( native_handle_t handle, bool owns_handle = true );

        /// <summary>
        /// The native handle.
        /// </summary>
        native_handle_t native;

       private:
        /// <summary>
        /// Creates a new handle object.
        /// </summary>
        /// <param name="handle">The native handle.</param>
        /// <param name="owns_handle">Whether this object owns the handle.</param>
        handle_t( native_handle_t handle, bool owns_handle );

        /// <summary>
        /// Whether the handle object owns the handle.
        /// </summary>
        bool owns_handle;
    };

    /// <summary>
    /// Structure defines the coordinates of the upper-left and lower-right corners of a rectangle.
    /// </summary>
    struct rectangle_t
    {
        /// <summary>
        /// Creates an empty rectangle object.
        /// </summary>
        rectangle_t() noexcept;

        /// <summary>
        /// Creates a new rectangle object.
        /// </summary>
        /// <param name="left">The x-coordinate of the upper-left corner.</param>
        /// <param name="top">The y-coordinate of the upper-left corner.</param>
        /// <param name="right">The x-coordinate of the lower-right corner.</param>
        /// <param name="bottom">The y-coordinate of the lower-right corner.</param>
        rectangle_t( std::int32_t left, std::int32_t top, std::int32_t right, std::int32_t bottom ) noexcept;

        /// <summary>
        /// The x-coordinate of the upper-left corner of the rectangle.
        /// </summary>
        std::int32_t left;

        /// <summary>
        /// The y-coordinate of the upper-left corner of the rectangle.
        /// </summary>
        std::int32_t top;

        /// <summary>
        /// The x-coordinate of the lower-right corner of the rectangle.
        /// </summary>
        std::int32_t right;

        /// <summary>
        /// The y-coordinate of the lower-right corner of the rectangle.
        /// </summary>
        std::int32_t bottom;

        /// <summary>
        /// Gets the height of the rectangle.
        /// </summary>
        /// <returns>The rectangle height.</returns>
        constexpr std::int32_t height() const noexcept;

        /// <summary>
        /// Sets the height of the rectangle.
        /// </summary>
        /// <param name="value">The new height.</param>
        constexpr void set_height( std::int32_t value ) noexcept;

        /// <summary>
        /// Gets the width of the rectangle.
        /// </summary>
        /// <returns>The rectangle width.</returns>
        constexpr std::int32_t width() const noexcept;

        /// <summary>
        /// Sets the width of the rectangle.
        /// </summary>
        /// <param name="value">The new width.</param>
        constexpr void set_width( std::int32_t value ) noexcept;
    };

    /// <summary>
    /// The point_t structure defines the x and y coordinates of a point.
    /// </summary>
    struct point_t
    {
        /// <summary>
        /// Creates an empty point object.
        /// </summary>
        point_t() noexcept;

        /// <summary>
        /// Creates a new point object.
        /// </summary>
        /// <param name="x">The x-coordinate of the point.</param>
        /// <param name="y">The y-coordinate of the point.</param>
        point_t( std::int32_t x, std::int32_t y ) noexcept;

        /// <summary>
        /// The x-coordinate of the point.
        /// </summary>
        std::int32_t x;

        /// <summary>
        /// The y-coordinate of the point.
        /// </summary>
        std::int32_t y;
    };
}  // namespace wincpp::core

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/core/win.inl"
#endif
