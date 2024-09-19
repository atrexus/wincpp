#pragma once

#include <string>

#include "memory_factory.hpp"
#include "module_factory.hpp"

namespace wincpp
{
    /// <summary>
    /// A class that offers functionality for working with processes.
    /// </summary>
    struct process_t final
    {
        friend class memory_factory;
        friend class module_factory;

        /// <summary>
        /// Opens a process by its name.
        /// </summary>
        /// <param name="name">The name of the process.</param>
        /// <param name="access">The access rights.</param>
        /// <returns>A unique pointer to the process.</returns>
        static std::unique_ptr< process_t > open( std::string_view name, std::uint32_t access = PROCESS_ALL_ACCESS );

        /// <summary>
        /// Opens a process by its id.
        /// </summary>
        /// <param name="id">The id of the process.</param>
        /// <param name="access">The access rights.</param>
        /// <returns>A unique pointer to the process.</returns>
        static std::unique_ptr< process_t > open( std::uint32_t id, std::uint32_t access = PROCESS_ALL_ACCESS );

        /// <summary>
        /// Gets a handle to the current process.
        /// </summary>
        static std::unique_ptr< process_t > current();

        /// <summary>
        /// The module factory object.
        /// </summary>
        module_factory module_factory;

        /// <summary>
        /// The memory factory object.
        /// </summary>
        memory_factory memory_factory;

        /// <summary>
        /// Gets the process id.
        /// </summary>
        constexpr std::uint32_t id() const noexcept
        {
            return entry.id;
        }

        /// <summary>
        /// Gets the process name.
        /// </summary>
        constexpr std::string_view name() const noexcept
        {
            return entry.name;
        }

        /// <summary>
        /// Gets the process handle.
        /// </summary>
        std::shared_ptr< core::handle_t > handle;

       private:
        /// <summary>
        /// Creates a new process object.
        /// </summary>
        /// <param name="handle">The handle to the process object.</param>
        /// <param name="id">The process id.</param>
        /// <param name="name">The process name.</param>
        /// <param name="type">The memory type.</param>
        explicit process_t( std::shared_ptr< core::handle_t > handle, const core::process_entry_t& entry, memory_type type ) noexcept;

        core::process_entry_t entry;
        
    };

}  // namespace wincpp

#include "memory/pointer.hpp"
#include "memory/region.hpp"