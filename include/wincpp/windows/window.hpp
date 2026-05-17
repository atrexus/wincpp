#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "wincpp/core/win.hpp"
#include "wincpp/thread_factory.hpp"

namespace wincpp
{
    /// <summary>
    /// Forward declaration of window_factory.
    /// </summary>
    class window_factory;
}  // namespace wincpp

namespace wincpp::windows
{
    /// <summary>
    /// Represents a window in a process.
    /// </summary>
    struct window_t
    {
        friend class wincpp::window_factory;

        /// <summary>
        /// The placement of the window.
        /// </summary>
        struct placement_t;

        /// <summary>
        /// The states of the window.
        /// </summary>
        enum class state_t : std::uint32_t;

        /// <summary>
        /// Gets the handle of the current window.
        /// </summary>
        /// <returns>The native window handle.</returns>
        core::native_window_handle_t handle() const noexcept;

        /// <summary>
        /// Gets the process ID of the window.
        /// </summary>
        /// <returns>The process identifier.</returns>
        std::uint32_t process_id() const;

        /// <summary>
        /// Gets the owner of the window.
        /// </summary>
        /// <returns>The owner window.</returns>
        std::optional< window_t > owner() const noexcept;

        /// <summary>
        /// Gets the title of the window.
        /// </summary>
        /// <returns>The window title.</returns>
        std::string title() const;

        /// <summary>
        /// Gets the class name of the window.
        /// </summary>
        /// <returns>The window class name.</returns>
        std::string class_name() const;

        /// <summary>
        /// Determines if the window is active (in the foreground).
        /// </summary>
        /// <returns>True if the window is active.</returns>
        bool is_active() const;

        /// <summary>
        /// Gets the placement of the window.
        /// </summary>
        /// <returns>The window placement.</returns>
        placement_t placement() const;

        /// <summary>
        /// Gets the thread that created the window.
        /// </summary>
        /// <returns>The thread that created the window.</returns>
        threads::thread_t thread() const;

        /// <summary>
        /// Creates a new window object.
        /// </summary>
        /// <param name="threads">The thread factory.</param>
        /// <param name="hwnd">The native window handle.</param>
        explicit window_t( const thread_factory& threads, core::native_window_handle_t hwnd ) noexcept;

       private:
        core::native_window_handle_t hwnd;
        thread_factory tds;
    };

    /// <summary>
    /// The placement of the window.
    /// </summary>
    struct window_t::placement_t
    {
        /// <summary>
        /// Creates a new window placement object.
        /// </summary>
        /// <param name="flags">The placement flags.</param>
        /// <param name="show_state">The show state.</param>
        /// <param name="min_position">The minimized position.</param>
        /// <param name="max_position">The maximized position.</param>
        /// <param name="normal_position">The normal position.</param>
        placement_t(
            std::uint32_t flags,
            state_t show_state,
            core::point_t min_position,
            core::point_t max_position,
            core::rectangle_t normal_position ) noexcept;

        /// <summary>
        /// Specifies flags that control the position of the minimized window and restoration behavior.
        /// </summary>
        std::uint32_t flags;

        /// <summary>
        /// The current show state of the window.
        /// </summary>
        state_t show_state;

        /// <summary>
        /// The coordinates of the window's upper-left corner when minimized.
        /// </summary>
        core::point_t min_position;

        /// <summary>
        /// The coordinates of the window's upper-left corner when maximized.
        /// </summary>
        core::point_t max_position;

        /// <summary>
        /// The window's coordinates when restored.
        /// </summary>
        core::rectangle_t normal_position;
    };

    /// <summary>
    /// The states of the window.
    /// </summary>
    enum class window_t::state_t : std::uint32_t
    {
        /// <summary>
        /// Hides the window and activates another window.
        /// </summary>
        hide_t = 0,

        /// <summary>
        /// Activates and displays the window.
        /// </summary>
        show_normal_t = 1,

        /// <summary>
        /// Equivalent to show_normal_t.
        /// </summary>
        normal_t = 1,

        /// <summary>
        /// Minimizes the window and activates the next top-level window.
        /// </summary>
        show_minimized_t = 2,

        /// <summary>
        /// Maximizes the specified window.
        /// </summary>
        show_maximized_t = 3,

        /// <summary>
        /// Displays a window in its most recent size and position.
        /// </summary>
        show_no_activate_t = 4,

        /// <summary>
        /// Activates and displays a window.
        /// </summary>
        show_t = 5,

        /// <summary>
        /// Minimizes the specified window.
        /// </summary>
        minimize_t = 6,

        /// <summary>
        /// Displays the window as a minimized window.
        /// </summary>
        show_min_no_activate_t = 7,

        /// <summary>
        /// Displays the window in its current size and position.
        /// </summary>
        show_na_t = 8,

        /// <summary>
        /// Activates and displays the window.
        /// </summary>
        restore_t = 9,

        /// <summary>
        /// Sets the show state based on the program that started the application.
        /// </summary>
        show_default_t = 10,

        /// <summary>
        /// Minimizes a window, even if the thread that owns the window is not responding.
        /// </summary>
        force_minimize_t = 11
    };
}  // namespace wincpp::windows

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/windows/window.inl"
#endif
