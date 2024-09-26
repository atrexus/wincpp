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

    /// <summary>
    /// Structure defines the coordinates of the upper-left and lower-right corners of a rectangle.
    /// </summary>
    struct rectangle_t
    {
        /// <summary>
        /// Creates a new rectangle object.
        /// </summary>
        rectangle_t( const RECT& rect ) noexcept;

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
        constexpr std::int32_t height() const
        {
            return bottom - top;
        }

        /// <summary>
        /// Sets the height of the rectangle.
        /// </summary>
        constexpr void set_height( std::int32_t value )
        {
            bottom = top + value;
        }

        /// <summary>
        /// Gets the width of the rectangle.
        /// </summary>
        constexpr std::int32_t width() const
        {
            return right - left;
        }

        /// <summary>
        /// Sets the width of the rectangle.
        /// </summary>
        constexpr void set_width( std::int32_t value )
        {
            right = left + value;
        }

        /// <summary>
        /// Implicit conversion to RECT.
        /// </summary>
        constexpr operator RECT() const
        {
            return { left, top, right, bottom };
        }
    };

    /// <summary>
    /// The point_t structure defines the x and y coordinates of a point.
    /// </summary>
    struct point_t
    {
        /// <summary>
        /// Creates a new point object.
        /// </summary>
        point_t( const POINT& point ) noexcept;

        /// <summary>
        /// The x-coordinate of the point.
        /// </summary>
        std::int32_t x;

        /// <summary>
        /// The y-coordinate of the point.
        /// </summary>
        std::int32_t y;

        /// <summary>
        /// Implicit conversion to POINT.
        /// </summary>
        constexpr operator POINT() const
        {
            return { x, y };
        }
    };
}  // namespace wincpp::core