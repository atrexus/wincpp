#pragma once

#include <format>
#include <utility>

#include "wincpp/modules/export.hpp"

namespace wincpp::modules
{
    inline module_t::export_t::export_t(
        std::shared_ptr< const module_t > target_module,
        const std::string_view name,
        const std::uintptr_t rva,
        const std::uint32_t ordinal ) noexcept
        : target_module( std::move( target_module ) ),
          export_name( name ),
          export_rva( rva ),
          ordinal_value( ordinal )
    {
    }

    inline std::string module_t::export_t::name() const noexcept
    {
        return export_name;
    }

    inline std::uintptr_t module_t::export_t::address() const noexcept
    {
        return target_module->address() + export_rva;
    }

    inline std::uint32_t module_t::export_t::ordinal() const noexcept
    {
        return ordinal_value;
    }

    inline std::shared_ptr< const module_t > module_t::export_t::module() const noexcept
    {
        return target_module;
    }

    inline std::string module_t::export_t::to_string() const
    {
        const auto symbol = export_name.empty() ? std::format( "#{}", ordinal_value ) : export_name;
        return std::format( "{} -> {} (0x{:X})", target_module->name(), symbol, address() );
    }

    inline std::ostream& operator<<( std::ostream& os, const module_t::export_t& exp )
    {
        return os << exp.to_string();
    }
}  // namespace wincpp::modules
