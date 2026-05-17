#pragma once

#include <utility>

#include "wincpp/modules/section.hpp"

namespace wincpp::modules
{
    inline module_t::section_t::section_t(
        std::shared_ptr< const module_t > mod,
        std::string name,
        const std::uintptr_t address,
        const std::size_t size ) noexcept
        : memory_t( mod->memory(), address, size ),
          mod( std::move( mod ) ),
          section_name( std::move( name ) )
    {
    }

    inline std::string_view module_t::section_t::name() const noexcept
    {
        return section_name;
    }
}  // namespace wincpp::modules
