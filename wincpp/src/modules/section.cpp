#include "modules/section.hpp"

#include "process.hpp"

namespace wincpp::modules
{
    module_t::section_t::section_t( const module_t &mod, const IMAGE_SECTION_HEADER &header ) noexcept
        : memory_t( mod.process->memory_factory, mod.base() + header.VirtualAddress, header.Misc.VirtualSize ),
          mod( mod ),
          header( header )
    {
    }

    std::string module_t::section_t::name() const noexcept
    {
        return std::string( reinterpret_cast< const char * >( header.Name ), sizeof( header.Name ) );
    }
}  // namespace wincpp::modules