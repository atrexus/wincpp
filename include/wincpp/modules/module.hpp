#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
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
        /// Enumerates the exports declared by this module and resolves forwarders to their final
        /// targets. Named aliases remain separate entries; ordinal-only exports have an empty name.
        /// </summary>
        /// <returns>The cached list of resolved exports.</returns>
        /// <exception cref="core::error">The export table is invalid or a forwarder cannot be resolved.</exception>
        const std::list< std::shared_ptr< module_t::export_t > >& exports() const;

        /// <summary>
        /// Finds an export by its exact, case-sensitive name and resolves any forwarders.
        /// </summary>
        /// <param name="name">The name of the export.</param>
        /// <returns>The resolved export, or nullptr if the module does not export the name.</returns>
        /// <exception cref="core::error">The export table is invalid or a forwarder cannot be resolved.</exception>
        std::shared_ptr< module_t::export_t > fetch_export( std::string_view name ) const;

        /// <summary>
        /// Finds an export by its PE ordinal and resolves any forwarders. The ordinal includes the
        /// export directory's base and is not a zero-based index into the function table.
        /// </summary>
        /// <param name="ordinal">The PE ordinal of the export.</param>
        /// <returns>The resolved export, or nullptr if the module does not export the ordinal.</returns>
        /// <exception cref="core::error">The export table is invalid or a forwarder cannot be resolved.</exception>
        std::shared_ptr< module_t::export_t > fetch_export( std::uint32_t ordinal ) const;

        /// <summary>
        /// Attempts to find and resolve an export by its exact, case-sensitive name.
        /// </summary>
        /// <param name="name">The name of the export.</param>
        /// <returns>The resolved export, or an error if lookup fails.</returns>
        core::result_t< std::shared_ptr< module_t::export_t > > try_fetch_export( std::string_view name ) const noexcept;

        /// <summary>
        /// Attempts to find and resolve an export by its PE ordinal.
        /// </summary>
        /// <param name="ordinal">The PE ordinal of the export.</param>
        /// <returns>The resolved export, or an error if lookup fails.</returns>
        core::result_t< std::shared_ptr< module_t::export_t > > try_fetch_export( std::uint32_t ordinal ) const noexcept;

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
        /// Gets an export by its exact, case-sensitive name and resolves any forwarders.
        /// </summary>
        /// <param name="name">The export name.</param>
        /// <returns>The export.</returns>
        /// <exception cref="core::error">The export is missing, invalid, or cannot be resolved.</exception>
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
        mutable bool _exports_loaded = false;

        struct export_directory_t
        {
            std::uint32_t virtual_address;
            std::uint32_t size;
            std::uint32_t ordinal_base;
            std::uint32_t function_count;
            std::uint32_t name_count;
            std::uint32_t functions_rva;
            std::uint32_t names_rva;
            std::uint32_t name_ordinals_rva;
        };

        struct export_target_t
        {
            std::shared_ptr< const module_t > module;
            std::uintptr_t rva;
        };

        std::optional< export_directory_t > export_directory() const;
        std::optional< std::uint32_t > find_export_ordinal( std::string_view name ) const;
        std::string read_export_string( std::uint32_t rva, std::uintptr_t end_rva ) const;
        std::optional< export_target_t > resolve_export_target(
            std::uint32_t ordinal,
            std::vector< std::pair< std::uintptr_t, std::uint32_t > >& forwarder_chain ) const;
        std::shared_ptr< export_t > resolve_export( std::uint32_t ordinal, std::string_view name ) const;
    };
}  // namespace wincpp::modules

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/modules/module.inl"
#endif
