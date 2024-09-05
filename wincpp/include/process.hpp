#pragma once

#include <string>

#include "modules/module_factory.hpp"

namespace wincpp
{
    /// <summary>
    /// Defines the types of memory manipulations.
    /// </summary>
    enum class memory_type
    {
        /// <summary>
        /// The memory is within the local process. Often, this is called "injected" or "Internal".
        /// </summary>
        local_t,

        /// <summary>
        /// The memory is not within the local process. Often this is called "remote" or "external".
        /// </summary>
        remote_t
    };

    /// <summary>
    /// A class that offers functionality for working with processes.
    /// </summary>
    class process_t final
    {
        std::uint32_t id;
        std::string name;
        std::shared_ptr< core::handle_t > handle;

        /// <summary>
        /// Creates a new process object.
        /// </summary>
        /// <param name="handle">The handle to the process object.</param>
        /// <param name="id">The process id.</param>
        /// <param name="name">The process name.</param>
        /// <param name="type">The memory type.</param>
        explicit process_t( std::shared_ptr< core::handle_t > handle, std::uint32_t id, std::string_view name, memory_type type );

        /// <summary>
        /// Creates a new process object.
        /// </summary>
        /// <param name="name">The handle to the process object..</param>
        /// <param name="type">The memory type.</param>
        explicit process_t( std::shared_ptr< core::handle_t > handle, memory_type type );

       public:
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
        static std::unique_ptr< process_t > current() noexcept;

        /// <summary>
        /// The module factory object.
        /// </summary>
        module_factory module_factory;
    };

}  // namespace wincpp