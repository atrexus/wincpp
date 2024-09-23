#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <optional>

namespace wincpp::windows
{
    /// <summary>
    /// Forward declare the window_t struct.
    /// </summary>
    struct window_t;
}  // namespace wincpp::windows

namespace wincpp
{
    struct process_t;

    /// <summary>
    /// Class providing tools for manipulating windows.
    /// </summary>
    class window_factory final
    {
        friend struct process_t;

        process_t *p;

        /// <summary>
        /// Creates a new window factory object.
        /// </summary>
        /// <param name="process">The process object.</param>
        explicit window_factory( process_t *p ) noexcept;

       public:
        /// <summary>
        /// Gets the main window of the process.
        /// </summary>
        std::optional< windows::window_t > main_window() const;

        /// <summary>
        /// Gets a list of windows in the process.
        /// </summary>
        std::vector< windows::window_t > windows() const;
    };
}  // namespace wincpp

#include "windows/window.hpp"