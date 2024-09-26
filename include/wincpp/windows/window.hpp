#pragma once

#include <optional>

#include "wincpp/core/win.hpp"

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
        friend class window_factory;

        /// <summary>
        /// The placement of the window.
        /// </summary>
        struct placement_t;

        /// <summary>
        /// The states of the window.
        /// </summary>
        enum class state_t;

        /// <summary>
        /// Gets the handle of the current window.
        /// </summary>
        HWND handle() const noexcept;

        /// <summary>
        /// Gets the process ID of the window.
        /// </summary>
        std::uint32_t process_id() const;

        /// <summary>
        /// Gets the owner of the window.
        /// </summary>
        std::optional< window_t > owner() const noexcept;

        /// <summary>
        /// Gets the title of the window.
        /// </summary>
        std::string title() const;

        /// <summary>
        /// Gets the class name of the window.
        /// </summary>
        std::string class_name() const;

        /// <summary>
        /// Determines if the window is active (in the foreground).
        /// </summary>
        bool is_active() const;

        /// <summary>
        /// Gets the placement of the window.
        /// </summary>
        placement_t placement() const;

       private:
        /// <summary>
        /// Creates a new window object.
        /// </summary>
        explicit window_t( HWND hwnd ) noexcept;

        HWND hwnd;
    };

    /// <summary>
    /// The placement of the window.
    /// </summary>
    struct window_t::placement_t
    {
        /// <summary>
        /// Creates a new window placement object.
        /// </summary>
        placement_t( const WINDOWPLACEMENT& placement ) noexcept;

        /// <summary>
        /// Specifies flags that control the position of the minimized window and the method by which the window is restored.
        /// </summary>
        std::uint32_t flags;

        /// <summary>
        /// The current show state of the window.
        /// </summary>
        state_t show_state;

        /// <summary>
        /// The coordinates of the window's upper-left corner when the window is minimized.
        /// </summary>
        core::point_t min_position;

        /// <summary>
        /// The coordinates of the window's upper-left corner when the window is maximized.
        /// </summary>
        core::point_t max_position;

        /// <summary>
        /// The window's coordinates when the window is in the restored position.
        /// </summary>
        core::rectangle_t normal_position;
    };

    /// <summary>
    /// The states of the window.
    /// </summary>
    enum class window_t::state_t
    {
        /// <summary>
        /// Hides the window and activates another window.
        /// Corresponds to SW_HIDE.
        /// </summary>
        hide_t = 0,

        /// <summary>
        /// Activates and displays the window. If the window is minimized or maximized, the system restores it to its original size and position.
        /// Corresponds to SW_SHOWNORMAL.
        /// </summary>
        show_normal_t = 1,

        /// <summary>
        /// Equivalent to show_normal_t. Activates and displays the window.
        /// Corresponds to SW_NORMAL.
        /// </summary>
        normal_t = 1,

        /// <summary>
        /// Minimizes the window and activates the next top-level window.
        /// Corresponds to SW_SHOWMINIMIZED.
        /// </summary>
        show_minimized_t = 2,

        /// <summary>
        /// Maximizes the specified window.
        /// Corresponds to SW_SHOWMAXIMIZED.
        /// </summary>
        show_maximized_t = 3,

        /// <summary>
        /// Displays a window in its most recent size and position. The window is not activated.
        /// Corresponds to SW_SHOWNOACTIVATE.
        /// </summary>
        show_no_activate_t = 4,

        /// <summary>
        /// Activates and displays a window. If the window is minimized or maximized, the system restores it to its original size and position.
        /// Corresponds to SW_SHOW.
        /// </summary>
        show_t = 5,

        /// <summary>
        /// Minimizes the specified window and activates the next top-level window.
        /// Corresponds to SW_MINIMIZE.
        /// </summary>
        minimize_t = 6,

        /// <summary>
        /// Displays the window as a minimized window. The window is not activated.
        /// Corresponds to SW_SHOWMINNOACTIVE.
        /// </summary>
        show_min_no_activate_t = 7,

        /// <summary>
        /// Displays the window in its current size and position. The window is not activated.
        /// Corresponds to SW_SHOWNA.
        /// </summary>
        show_na_t = 8,

        /// <summary>
        /// Activates and displays the window. If the window is minimized or maximized, the system restores it to its original size and position.
        /// Corresponds to SW_RESTORE.
        /// </summary>
        restore_t = 9,

        /// <summary>
        /// Sets the show state based on the program that started the application.
        /// Corresponds to SW_SHOWDEFAULT.
        /// </summary>
        show_default_t = 10,

        /// <summary>
        /// Minimizes a window, even if the thread that owns the window is not responding. This flag should only be used when minimizing windows from
        /// a different thread. Corresponds to SW_FORCEMINIMIZE.
        /// </summary>
        force_minimize_t = 11
    };

}  // namespace wincpp::windows