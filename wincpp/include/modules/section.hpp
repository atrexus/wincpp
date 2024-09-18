#pragma once

#include "module.hpp"
#include "memory/memory.hpp"
#include "patterns/scanner.hpp"

namespace wincpp::modules
{
    /// <summary>
    /// Class representing a section of a module.
    /// </summary>
    struct module_t::section_t : public memory::memory_t
    {
        friend struct module_t;

        /// <summary>
        /// Gets the name of the section.
        /// </summary>
        std::string name() const noexcept;

       private:
        /// <summary>
        /// Creates a new section object.
        /// </summary>
        /// <param name="mod">The mod object.</param>
        /// <param name="header">The section header.</param>
        explicit section_t( const module_t& mod, const IMAGE_SECTION_HEADER& header ) noexcept;

        module_t mod;
        IMAGE_SECTION_HEADER header;
    };
}  // namespace wincpp::modules