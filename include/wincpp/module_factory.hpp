#pragma once

#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "wincpp/core/error.hpp"
#include "wincpp/core/snapshot.hpp"

namespace wincpp::modules
{
    /// <summary>
    /// Forward declaration of module_t.
    /// </summary>
    struct module_t;
}  // namespace wincpp::modules

namespace wincpp
{
    struct process_t;

    /// <summary>
    /// Class providing tools for manipulating modules and libraries.
    /// </summary>
    class module_factory final
    {
        friend struct process_t;
        friend struct modules::module_t;

        process_t* p;
        mutable std::vector< std::shared_ptr< modules::module_t > > module_list;
        mutable std::unordered_set< std::uintptr_t > module_keys;

        /// <summary>
        /// Creates a new module factory object.
        /// </summary>
        /// <param name="process">The process object.</param>
        explicit module_factory( process_t* p ) noexcept;

        std::shared_ptr< modules::module_t > fetch_module( std::string_view name, std::string_view importing_module ) const;

       public:
        /// <summary>
        /// Gets a list of modules in the process.
        /// </summary>
        /// <returns>The list of modules.</returns>
        const std::vector< std::shared_ptr< modules::module_t > >& modules() const;

        /// <summary>
        /// Gets a list of modules in the process, optionally rebuilding the cached module list first.
        /// </summary>
        /// <param name="refresh">Whether the cached module list should be cleared before enumeration.</param>
        /// <returns>The list of modules.</returns>
        const std::vector< std::shared_ptr< modules::module_t > >& modules( bool refresh ) const;

        /// <summary>
        /// Clears the cached module list so that the next module enumeration uses a fresh snapshot.
        /// </summary>
        void refresh() const;

        /// <summary>
        /// Gets the main module of the process.
        /// </summary>
        /// <returns>The main module.</returns>
        const modules::module_t& main_module() const;

        /// <summary>
        /// Finds a loaded module by base name. Matching ignores ASCII case, the .dll extension
        /// is optional, and API-set contract names resolve to their loaded host module.
        /// </summary>
        /// <param name="name">The module name, path, or API-set contract name.</param>
        /// <returns>The loaded module, or nullptr if no matching module is found.</returns>
        std::shared_ptr< modules::module_t > fetch_module( std::string_view name ) const;

        /// <summary>
        /// Attempts to find a loaded module using the same name matching rules as fetch_module.
        /// </summary>
        /// <param name="name">The module name, path, or API-set contract name.</param>
        /// <returns>The loaded module, or an error if lookup fails.</returns>
        core::result_t< std::shared_ptr< modules::module_t > > try_fetch_module( std::string_view name ) const noexcept;

        /// <summary>
        /// Gets a loaded module using the same name matching rules as fetch_module.
        /// </summary>
        /// <param name="name">The module name, path, or API-set contract name.</param>
        /// <returns>The module.</returns>
        /// <exception cref="core::error">No matching module can be found.</exception>
        const modules::module_t& operator[]( std::string_view name ) const;
    };
}  // namespace wincpp

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/module_factory.inl"
#endif
