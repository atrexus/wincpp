#pragma once

#include "core/win.hpp"

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
        explicit module_factory( process_t *p );

       public:
    };
}  // namespace wincpp::modules