#pragma once

// clang-format off
#include "wincpp/core/snapshot.hpp"
#include "wincpp/memory/memory.hpp"
#include "wincpp/modules/object.hpp"
// clang-format on

#include <Psapi.h>

#include <optional>
#include <vector>

namespace wincpp
{
    struct process_t;
    class module_factory;
}  // namespace wincpp

namespace wincpp::modules
{
    class module_list;

    /// <summary>
    /// Class representing a module in a process.
    /// </summary>
    struct module_t : public memory::memory_t
    {
        friend class module_list;
        friend class module_factory;

        /// <summary>
        /// Represents an export of a module.
        /// </summary>
        struct export_t;

        /// <summary>
        /// Represents a section of a module.
        /// </summary>
        struct section_t;

        /// <summary>
        /// Gets the name of the module.
        /// </summary>
        std::string name() const noexcept;

        /// <summary>
        /// Gets the memory address of the entry point of the module.
        /// </summary>
        std::uintptr_t entry_point() const noexcept;

        /// <summary>
        /// Gets the full path to the module.
        /// </summary>
        std::string path() const noexcept;

        /// <summary>
        /// Gets the export by its name.
        /// </summary>
        /// <param name="name">The name of the export.</param>
        /// <returns>The export.</returns>
        std::optional< export_t > fetch_export( const std::string_view name ) const;

        /// <summary>
        /// Gets the section by its name.
        /// </summary>
        /// <param name="name">The name of the section.</param>
        /// <returns>The section.</returns>
        std::optional< section_t > fetch_section( const std::string_view name ) const;

        /// <summary>
        /// Locates all objects in the module by their mangled name.
        /// </summary>
        /// <param name="mangled">The mangled name.</param>
        /// <returns>A list of objects.</returns>
        std::vector< std::shared_ptr< rtti::object_t > > fetch_objects( const std::string_view mangled ) const;

        /// <summary>
        /// Gets the export by its name.
        /// </summary>
        export_t operator[]( const std::string_view name ) const;

       private:
        /// <summary>
        /// Creates a new module object.
        /// </summary>
        /// <param name="factory">The memory factory.</param>
        /// <param name="entry">The module entry.</param>
        explicit module_t( const memory_factory &factory, const core::module_entry_t &entry ) noexcept;

        core::module_entry_t entry;
        MODULEINFO info;

        const IMAGE_DOS_HEADER *dos_header;
        const IMAGE_NT_HEADERS *nt_headers;

        std::shared_ptr< std::uint8_t[] > buffer;
    };

    /// <summary>
    /// Represents a list of modules in the remote process. Contains the iterator for ToolHelp32Snapshot.
    /// </summary>
    class module_list final
    {
        friend class module_factory;

        core::snapshot< core::snapshot_kind::module_t > snapshot;
        process_t *process;

        /// <summary>
        /// Creates a new module list object.
        /// </summary>
        /// <param name="process">The process object.</param>
        explicit module_list( process_t *process ) noexcept;

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

    class module_list::iterator final
    {
        friend class module_list;

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