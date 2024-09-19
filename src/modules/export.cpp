#include "modules/export.hpp"

#include <format>

namespace wincpp::modules
{
    std::string module_t::export_t::name() const noexcept
    {
        return export_name;
    }

    std::uintptr_t module_t::export_t::virtual_address() const noexcept
    {
        return mod.address() + rva;
    }

    std::uint16_t module_t::export_t::ordinal() const noexcept
    {
        return ordinal_value;
    }

    module_t::export_t::export_t(
        const module_t& mod,
        const std::string_view name,
        const std::uintptr_t address,
        const std::uint16_t ordinal ) noexcept
        : export_name( name ),
          rva( address ),
          ordinal_value( ordinal ),
          mod( mod )
    {
    }

    std::string module_t::export_t::to_string() const
    {
        return std::format( "{0} -> {1} (0x{2:X})", mod.name(), name(), virtual_address() );
    }

    std::ostream &operator<<( std::ostream &os, const module_t::export_t &exp )
    {
        return os << exp.to_string();
    }
}  // namespace wincpp::modules