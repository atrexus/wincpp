#pragma once

#include <string>
#include <string_view>

#include "wincpp/memory/memory.hpp"
#include "wincpp/modules/module.hpp"

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
        /// <returns>The section name.</returns>
        std::string_view name() const noexcept;

       private:
        /// <summary>
        /// Creates a new section object.
        /// </summary>
        /// <param name="mod">The mod object.</param>
        /// <param name="name">The section name.</param>
        /// <param name="address">The section address.</param>
        /// <param name="size">The section size.</param>
        explicit section_t( std::shared_ptr< const module_t > mod, std::string name, std::uintptr_t address, std::size_t size ) noexcept;

        std::shared_ptr< const module_t > mod;
        std::string section_name;
    };
}  // namespace wincpp::modules

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/modules/section.inl"
#endif
