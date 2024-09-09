#pragma once

// clang-format off
#include "core/snapshot.hpp"
// clang-format on

#include <Psapi.h>

#include <optional>

namespace wincpp
{
    struct process_t;
    class module_factory;
}  // namespace wincpp

namespace wincpp::modules
{
    class module_list_t;

    /// <summary>
    /// Class representing a module in a process.
    /// </summary>
    struct module_t final
    {
        friend class module_list_t;

        /// <summary>
        /// Represents an export of a module.
        /// </summary>
        struct export_t;

        /// <summary>
        /// Gets the name of the module.
        /// </summary>
        std::string name() const noexcept;

        /// <summary>
        /// Gets the memory address where the module is loaded.
        /// </summary>
        std::uintptr_t base() const noexcept;

        /// <summary>
        /// Gets the memory address of the entry point of the module.
        /// </summary>
        std::uintptr_t entry_point() const noexcept;

        /// <summary>
        /// Gets the full path to the module.
        /// </summary>
        std::string path() const noexcept;

        /// <summary>
        /// Gets the size of the module in bytes.
        /// </summary>
        std::size_t size() const noexcept;

        /// <summary>
        /// Gets the export by its name.
        /// </summary>
        /// <param name="name">The name of the export.</param>
        /// <returns>The export.</returns>
        std::optional< export_t > fetch_export( const std::string_view name ) const;

        /// <summary>
        /// Gets the export by its name.
        /// </summary>
        export_t operator[]( const std::string_view name ) const;

       private:
        /// <summary>
        /// Creates a new module object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="entry">The module entry.</param>
        explicit module_t( process_t *process, const core::module_entry_t& entry ) noexcept;

        process_t *process;
        const core::module_entry_t entry;
        MODULEINFO info;

        const IMAGE_DOS_HEADER *dos_header;
        const IMAGE_NT_HEADERS *nt_headers;

        std::shared_ptr< std::uint8_t[] > buffer;
    };

    /// <summary>
    /// Represents a list of modules in the remote process. Contains the iterator for ToolHelp32Snapshot.
    /// </summary>
    class module_list_t final
    {
        friend class module_factory;

        core::snapshot< core::snapshot_kind::module_t > snapshot;
        process_t *process;

        /// <summary>
        /// Creates a new module list object.
        /// </summary>
        /// <param name="process">The process object.</param>
        explicit module_list_t( process_t *process ) noexcept;

       public:
        /// <summary>
        /// The iterator for the module list.
        /// </summary>
        class iterator;

        /// <summary>
        /// Gets the begin iterator for the module list.
        /// </summary>
        iterator begin() const noexcept;

        /// <summary>
        /// Gets the end iterator for the module list.
        /// </summary>
        iterator end() const noexcept;
    };

    class module_list_t::iterator final
    {
        friend class module_list_t;

        process_t *process;
        core::snapshot< core::snapshot_kind::module_t >::iterator it;

        /// <summary>
        /// Creates a new module iterator object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="it">The module iterator.</param>
        iterator( process_t *process, const core::snapshot< core::snapshot_kind::module_t >::iterator &it ) noexcept;

       public:
        /// <summary>
        /// Gets the module object.
        /// </summary>
        /// <returns>The module object.</returns>
        module_t operator*() const noexcept;

        /// <summary>
        /// Moves to the next module.
        /// </summary>
        /// <returns>The next module iterator.</returns>
        iterator &operator++();

        /// <summary>
        /// Compares two module iterators.
        /// </summary>
        /// <param name="other">The other module iterator.</param>
        /// <returns>True if the iterators are equal, false otherwise.</returns>
        bool operator==( const iterator &other ) const noexcept;

        /// <summary>
        /// Compares two module iterators.
        /// </summary>
        /// <param name="other">The other module iterator.</param>
        /// <returns>True if the iterators are not equal, false otherwise.</returns>
        bool operator!=( const iterator &other ) const noexcept;
    };
}  // namespace wincpp::modules