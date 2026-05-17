#pragma once

#include <ostream>

#include "wincpp/modules/module.hpp"

namespace wincpp::modules
{
    /// <summary>
    /// Represents an exported symbol in a module.
    /// </summary>
    struct module_t::export_t final
    {
        friend struct module_t;

        /// <summary>
        /// Gets the name of the export.
        /// </summary>
        /// <returns>The export name.</returns>
        std::string name() const noexcept;

        /// <summary>
        /// Gets the virtual address of the export.
        /// </summary>
        /// <returns>The export address.</returns>
        std::uintptr_t address() const noexcept;

        /// <summary>
        /// Gets the ordinal of the export.
        /// </summary>
        /// <returns>The export ordinal.</returns>
        std::uint16_t ordinal() const noexcept;

        /// <summary>
        /// Gets the module that owns the export.
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
        /// <param name="mod">The mod object.</param>
        /// <param name="name">The name of the export.</param>
        /// <param name="address">The address of the export.</param>
        /// <param name="ordinal">The ordinal of the export.</param>
        explicit export_t( std::shared_ptr< const module_t > mod, std::string_view name, std::uintptr_t address, std::uint16_t ordinal ) noexcept;

        std::shared_ptr< const module_t > mod;
        std::string export_name;
        std::uintptr_t rva;
        std::uint16_t ordinal_value;
    };
}  // namespace wincpp::modules

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/modules/export.inl"
#endif
