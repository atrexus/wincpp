#pragma once

#include <memory>
#include <string_view>

#ifndef WINCPP_SUPPRESS_AUTO_INL
#define WINCPP_SUPPRESS_AUTO_INL
#define WINCPP_RESTORE_AUTO_INL
#endif

#include "wincpp/core/error.hpp"
#include "wincpp/core/snapshot.hpp"
#include "wincpp/core/win.hpp"
#include "wincpp/memory_factory.hpp"
#include "wincpp/module_factory.hpp"
#include "wincpp/thread_factory.hpp"
#include "wincpp/window_factory.hpp"

#ifdef WINCPP_RESTORE_AUTO_INL
#undef WINCPP_SUPPRESS_AUTO_INL
#undef WINCPP_RESTORE_AUTO_INL
#endif

namespace wincpp
{
    /// <summary>
    /// A class that offers functionality for working with processes.
    /// </summary>
    struct process_t final
    {
        friend class memory_factory;
        friend class module_factory;
        friend class window_factory;
        friend class thread_factory;

        /// <summary>
        /// Opens a process by its name.
        /// </summary>
        /// <param name="name">The name of the process.</param>
        /// <param name="access">The access rights.</param>
        /// <returns>A unique pointer to the process.</returns>
        static std::unique_ptr< process_t > open( std::string_view name, core::process_access_t access = core::process_access_t::all_t );

        /// <summary>
        /// Attempts to open a process by its name without throwing an exception.
        /// </summary>
        /// <param name="name">The name of the process.</param>
        /// <param name="access">The access rights.</param>
        /// <returns>The process when it was opened, or an error describing why it failed.</returns>
        static core::result_t< std::unique_ptr< process_t > > try_open(
            std::string_view name,
            core::process_access_t access = core::process_access_t::all_t ) noexcept;

        /// <summary>
        /// Opens a process by its id.
        /// </summary>
        /// <param name="id">The id of the process.</param>
        /// <param name="access">The access rights.</param>
        /// <returns>A unique pointer to the process.</returns>
        static std::unique_ptr< process_t > open( std::uint32_t id, core::process_access_t access = core::process_access_t::all_t );

        /// <summary>
        /// Attempts to open a process by its id without throwing an exception.
        /// </summary>
        /// <param name="id">The id of the process.</param>
        /// <param name="access">The access rights.</param>
        /// <returns>The process when it was opened, or an error describing why it failed.</returns>
        static core::result_t< std::unique_ptr< process_t > > try_open(
            std::uint32_t id,
            core::process_access_t access = core::process_access_t::all_t ) noexcept;

        /// <summary>
        /// Gets a handle to the current process.
        /// </summary>
        /// <returns>The current process.</returns>
        static std::unique_ptr< process_t > current();

        /// <summary>
        /// Attempts to get a handle to the current process without throwing an exception.
        /// </summary>
        /// <returns>The current process when it was created, or an error describing why it failed.</returns>
        static core::result_t< std::unique_ptr< process_t > > try_current() noexcept;

        /// <summary>
        /// The module factory object.
        /// </summary>
        module_factory module_factory;

        /// <summary>
        /// The memory factory object.
        /// </summary>
        memory_factory memory_factory;

        /// <summary>
        /// The window factory object.
        /// </summary>
        window_factory window_factory;

        /// <summary>
        /// The thread factory object.
        /// </summary>
        thread_factory thread_factory;

        /// <summary>
        /// Gets the process id.
        /// </summary>
        /// <returns>The process id.</returns>
        constexpr std::uint32_t id() const noexcept;

        /// <summary>
        /// Gets the process name.
        /// </summary>
        /// <returns>The process name.</returns>
        constexpr std::string_view name() const noexcept;

        /// <summary>
        /// Gets the process handle.
        /// </summary>
        std::shared_ptr< core::handle_t > handle;

       private:
        /// <summary>
        /// Creates a new process object.
        /// </summary>
        /// <param name="handle">The handle to the process object.</param>
        /// <param name="entry">The process entry.</param>
        /// <param name="type">The memory type.</param>
        explicit process_t( std::shared_ptr< core::handle_t > handle, const core::process_entry_t& entry, memory_type type ) noexcept;

        core::process_entry_t entry;
    };
}  // namespace wincpp

#ifndef WINCPP_SUPPRESS_AUTO_INL
#define WINCPP_SUPPRESS_AUTO_INL
#define WINCPP_RESTORE_POST_DECL_AUTO_INL
#endif

#include "wincpp/memory/allocation.hpp"
#include "wincpp/memory/pointer.hpp"
#include "wincpp/memory/region.hpp"
#include "wincpp/modules/export.hpp"
#include "wincpp/modules/module.hpp"
#include "wincpp/modules/section.hpp"
#include "wincpp/threads/thread.hpp"
#include "wincpp/windows/window.hpp"

#ifdef WINCPP_RESTORE_POST_DECL_AUTO_INL
#undef WINCPP_SUPPRESS_AUTO_INL
#undef WINCPP_RESTORE_POST_DECL_AUTO_INL
#endif

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/wincpp.inl"
#endif
