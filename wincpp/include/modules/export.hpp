#include <ostream>

#include "module.hpp"

namespace wincpp::modules
{
    struct module_t::export_t final
    {
        friend struct module_t;

        /// <summary>
        /// Gets the name of the export.
        /// </summary>
        std::string name() const noexcept;

        /// <summary>
        /// Gets the virtual address of the export.
        /// </summary>
        std::uintptr_t virtual_address() const noexcept;

        /// <summary>
        /// Gets the ordinal of the export.
        /// </summary>
        std::uint16_t ordinal() const noexcept;

        /// <summary>
        /// Returns a string representation of the export.
        /// </summary>
        std::string to_string() const;

        /// <summary>
        /// Writes the export to the output stream.
        /// </summary>
        friend std::ostream &operator<<( std::ostream &os, const export_t &exp );

       private:
        /// <summary>
        /// Creates a new export object.
        /// </summary>
        /// <param name="mod">The mod object.</param>
        /// <param name="name">The name of the export.</param>
        /// <param name="address">The address of the export.</param>
        /// <param name="ordinal">The ordinal of the export.</param>
        explicit export_t( const module_t &mod, const std::string_view name, const std::uintptr_t address, const std::uint16_t ordinal ) noexcept;

        module_t mod;
        std::string export_name;
        std::uintptr_t rva;
        std::uint16_t ordinal_value;
    };
}  // namespace wincpp::modules