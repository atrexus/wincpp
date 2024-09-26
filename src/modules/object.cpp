#include "wincpp/modules/object.hpp"

#include <DbgHelp.h>

#pragma comment( lib, "DbgHelp.lib" )

namespace wincpp::modules
{
    module_t::object_t::object_t( const module_t& module, std::uintptr_t vtable_address, const rtti::complete_object_locator_t& col ) noexcept
        : mod( module ),
          vtable_address( vtable_address ),
          col( col )
    {
    }

    rtti::type_descriptor_t module_t::object_t::type_descriptor() const noexcept
    {
        rtti::type_descriptor_t type{};

        // Read the type descriptor from the module.
        const auto buffer = mod.read( mod.address() + col.type_descriptor_offset, BUFSIZ );

        const auto vtable = *reinterpret_cast< std::uintptr_t* >( buffer.get() );
        const auto signature = *reinterpret_cast< std::uint32_t* >( buffer.get() + sizeof( std::uintptr_t ) );
        const std::string name = reinterpret_cast< const char* >( buffer.get() + 2 * sizeof( std::uintptr_t ) );

        return { vtable, signature, name };
    }

    std::string module_t::object_t::name() const noexcept
    {
        const auto type = type_descriptor();

        char demangled_name[ BUFSIZ ];

        if ( UnDecorateSymbolName( type.name.c_str(), demangled_name, sizeof( demangled_name ), UNDNAME_NAME_ONLY ) == 0 )
        {
            return type.name;
        }

        return demangled_name;
    }

    std::uintptr_t module_t::object_t::vtable() const noexcept
    {
        return vtable_address;
    }

}  // namespace wincpp::modules