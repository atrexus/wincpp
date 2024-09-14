#pragma once

// clang-format off
#include "core/win.hpp"
// clang-format on

namespace wincpp::modules
{
    /// <summary>
    /// Forward declaration of module_t.
    /// </summary>
    struct module_t;

    /// <summary>
    /// Forward declaration of module_list.
    /// </summary>
    class module_list;

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

        process_t *p;

        /// <summary>
        /// Creates a new module factory object.
        /// </summary>
        /// <param name="process">The process object.</param>
        explicit module_factory( process_t *p ) noexcept;

       public:
        /// <summary>
        /// Gets a list of modules in the process.
        /// </summary>
        /// <returns>The list of modules.</returns>
        modules::module_list modules() const;

        /// <summary>
        /// Gets the main module of the process.
        /// </summary>
        modules::module_t main_module() const;

        /// <summary>
        /// Gets a module by its name.
        /// </summary>
        /// <param name="name">The name of the module.</param>
        /// <returns>The module.</returns>
        modules::module_t fetch_module( const std::string_view name ) const;

        /// <summary>
        /// Gets a module by its name.
        /// </summary>
        modules::module_t operator[]( const std::string_view name ) const;
    };
}  // namespace wincpp

#include "modules/module.hpp"
#include "modules/export.hpp"
#include "modules/section.hpp"