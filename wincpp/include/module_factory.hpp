#pragma once

#include "core/win.hpp"
#include "modules/module_list.hpp"

namespace wincpp
{
    class process_t;

    /// <summary>
    /// Class providing tools for manipulating modules and libraries.
    /// </summary>
    class module_factory final
    {
        friend class process_t;

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
        modules::module_list_t list() const;
    };
}  // namespace wincpp