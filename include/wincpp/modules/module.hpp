#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "wincpp/core/error.hpp"
#include "wincpp/core/snapshot.hpp"
#include "wincpp/memory/memory.hpp"
#include "wincpp/modules/object.hpp"

namespace wincpp
{
    class memory_factory;
    struct process_t;
    class module_factory;
}  // namespace wincpp

namespace wincpp::modules
{
    /// <summary>
    /// Class representing a module in a process.
    /// </summary>
    struct module_t : public std::enable_shared_from_this< module_t >, memory::memory_t
    {
        friend class wincpp::module_factory;

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
        /// <returns>The module name.</returns>
        std::string_view name() const noexcept;

        /// <summary>
        /// Gets the memory address of the entry point of the module.
        /// </summary>
        /// <returns>The entry point address.</returns>
        std::uintptr_t entry_point() const noexcept;

        /// <summary>
        /// Gets the full path to the module.
        /// </summary>
        /// <returns>The module path.</returns>
        std::string path() const noexcept;

        /// <summary>
        /// Gets the list of exports in the module.
        /// </summary>
        /// <returns>The export list.</returns>
        const std::list< std::shared_ptr< module_t::export_t > >& exports() const;

        /// <summary>
        /// Gets the export by its name.
        /// </summary>
        /// <param name="name">The name of the export.</param>
        /// <returns>The export.</returns>
        std::shared_ptr< module_t::export_t > fetch_export( std::string_view name ) const;

        /// <summary>
        /// Attempts to get the export by its name without throwing an exception.
        /// </summary>
        /// <param name="name">The name of the export.</param>
        /// <returns>The export when it was found, or an error describing why it failed.</returns>
        core::result_t< std::shared_ptr< module_t::export_t > > try_fetch_export( std::string_view name ) const noexcept;

        /// <summary>
        /// Gets the list of sections in the module.
        /// </summary>
        /// <returns>The section list.</returns>
        const std::list< std::shared_ptr< module_t::section_t > >& sections() const;

        /// <summary>
        /// Gets the section by its name.
        /// </summary>
        /// <param name="name">The name of the section.</param>
        /// <returns>The section.</returns>
        std::shared_ptr< module_t::section_t > fetch_section( std::string_view name ) const;

        /// <summary>
        /// Locates all objects in the module by their mangled name.
        /// </summary>
        /// <param name="mangled">The mangled name.</param>
        /// <returns>A list of objects.</returns>
        std::vector< std::shared_ptr< rtti::object_t > > fetch_objects( std::string_view mangled ) const;

        /// <summary>
        /// Gets the export by its name.
        /// </summary>
        /// <param name="name">The export name.</param>
        /// <returns>The export.</returns>
        const export_t& operator[]( std::string_view name ) const;

       private:
        /// <summary>
        /// Creates a new module object.
        /// </summary>
        /// <param name="factory">The memory factory.</param>
        /// <param name="entry">The module entry.</param>
        explicit module_t( const memory_factory& factory, const core::module_entry_t& entry );

        core::module_entry_t entry;
        std::uintptr_t entry_point_address;
        std::uintptr_t nt_headers_offset;
        std::uint16_t section_count;
        std::shared_ptr< std::uint8_t[] > header_buffer;

        mutable std::list< std::shared_ptr< module_t::export_t > > _exports;
        mutable std::list< std::shared_ptr< module_t::section_t > > _sections;
    };
}  // namespace wincpp::modules

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/modules/module.inl"
#endif
