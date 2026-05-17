#pragma once

#include <format>
#include <utility>

#include "wincpp/modules/export.hpp"

namespace wincpp::modules
{
    inline module_t::export_t::export_t(
        std::shared_ptr< const module_t > mod,
        const std::string_view name,
        const std::uintptr_t address,
        const std::uint16_t ordinal ) noexcept
        : mod( std::move( mod ) ),
          export_name( name ),
          rva( address ),
          ordinal_value( ordinal )
    {
    }

    inline std::string module_t::export_t::name() const noexcept
    {
        return export_name;
    }

    inline std::uintptr_t module_t::export_t::address() const noexcept
    {
        return module()->address() + rva;
    }

    inline std::uint16_t module_t::export_t::ordinal() const noexcept
    {
        return ordinal_value;
    }

    inline std::shared_ptr< const module_t > module_t::export_t::module() const noexcept
    {
        return mod;
    }

    inline std::string module_t::export_t::to_string() const
    {
        return std::format( "{} -> {} (0x{:X})", mod->name(), name(), address() );
    }

    inline std::ostream& operator<<( std::ostream& os, const module_t::export_t& exp )
    {
        return os << exp.to_string();
    }
}  // namespace wincpp::modules
