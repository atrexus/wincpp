#pragma once

#include "wincpp/modules/module.hpp"
#include "wincpp/modules/object.hpp"

namespace wincpp::modules::rtti
{
    inline object_t::object_t(
        const module_t* const module,
        const std::uintptr_t vtable_address,
        const rtti::complete_object_locator_t& col ) noexcept
        : mod( module ),
          vtable_address( vtable_address ),
          col( col )
    {
    }

    inline rtti::type_descriptor_t object_t::type_descriptor() const noexcept
    {
        constexpr auto buffer_size = 4096;
        const auto buffer = mod->read( static_cast< std::uintptr_t >( col.type_descriptor_offset ), buffer_size );
        const auto vtable = *reinterpret_cast< const std::uintptr_t* >( buffer.get() );
        const auto spare = *reinterpret_cast< const std::uintptr_t* >( buffer.get() + sizeof( std::uintptr_t ) );
        const auto name = reinterpret_cast< const char* >( buffer.get() + sizeof( std::uintptr_t ) * 2 );

        return type_descriptor_t{ .type_info_vftable = vtable, .spare = spare, .name = name };
    }

    inline std::string object_t::name() const noexcept
    {
        return type_descriptor().name;
    }

    inline std::uintptr_t object_t::vtable() const noexcept
    {
        return vtable_address;
    }
}  // namespace wincpp::modules::rtti
