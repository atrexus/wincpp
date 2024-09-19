#include "modules/section.hpp"

#include "process.hpp"

#ifdef max
#undef max
#endif  // max

namespace wincpp::modules
{
    module_t::section_t::section_t( const module_t &mod, const IMAGE_SECTION_HEADER &header ) noexcept
        : memory_t( mod.factory, mod.address() + header.VirtualAddress, std::max( header.SizeOfRawData, header.Misc.VirtualSize ) ),
          mod( mod ),
          header( header )
    {
    }

    std::string module_t::section_t::name() const noexcept
    {
        return std::string( reinterpret_cast< const char * >( header.Name ), sizeof( header.Name ) );
    }
}  // namespace wincpp::modules