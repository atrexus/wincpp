#pragma once

#include <ostream>

#include "wincpp/modules/module.hpp"

namespace wincpp::modules
{
    /// <summary>
    /// Represents an export resolved from a module. A forwarded export retains the name and
    /// ordinal assigned by its source module, while its module and address identify the final target.
    /// </summary>
    struct module_t::export_t final
    {
        friend struct module_t;

        /// <summary>
        /// Gets the export name from the source module.
        /// </summary>
        /// <returns>The export name, or an empty string when the export was obtained only by ordinal.</returns>
        std::string name() const noexcept;

        /// <summary>
        /// Gets the resolved virtual address of the export.
        /// </summary>
        /// <returns>The export address.</returns>
        std::uintptr_t address() const noexcept;

        /// <summary>
        /// Gets the ordinal assigned by the source module.
        /// </summary>
        /// <returns>The source export ordinal.</returns>
        std::uint32_t ordinal() const noexcept;

        /// <summary>
        /// Gets the module containing the resolved implementation.
        /// </summary>
        /// <returns>The module.</returns>
        std::shared_ptr< const module_t > module() const noexcept;

        /// <summary>
        /// Returns a string representation of the export.
        /// </summary>
        /// <returns>The export string.</returns>
        std::string to_string() const;

        /// <summary>
        /// Writes the export to the output stream.
        /// </summary>
        /// <param name="os">The output stream.</param>
        /// <param name="exp">The export object.</param>
        /// <returns>The output stream.</returns>
        friend std::ostream& operator<<( std::ostream& os, const export_t& exp );

       private:
        /// <summary>
        /// Creates a new export object.
        /// </summary>
        /// <param name="target_module">The module containing the resolved implementation.</param>
        /// <param name="name">The export name from the source module.</param>
        /// <param name="rva">The export RVA in the target module.</param>
        /// <param name="ordinal">The export ordinal from the source module.</param>
        explicit export_t(
            std::shared_ptr< const module_t > target_module,
            std::string_view name,
            std::uintptr_t rva,
            std::uint32_t ordinal ) noexcept;

        std::shared_ptr< const module_t > target_module;
        std::string export_name;
        std::uintptr_t export_rva;
        std::uint32_t ordinal_value;
    };
}  // namespace wincpp::modules

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/modules/export.inl"
#endif
