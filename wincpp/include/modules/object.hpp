#pragma once

#include "module.hpp"

namespace wincpp::modules
{
    /// <summary>
    /// Contains all structures for MSVC RTTI.
    /// </summary>
    namespace rtti
    {
        /// <summary>
        /// Represents an MSVC type descriptor.
        /// </summary>
        struct type_descriptor_t
        {
            /// <summary>
            /// Address of the `type_info` object.
            /// </summary>
            const std::uintptr_t type_info_vftable;

            /// <summary>
            /// An unused field. Always 0.
            /// </summary>
            std::uintptr_t spare;

            /// <summary>
            /// The mangled name of the type.
            /// </summary>
            char name[];
        };

        struct complete_object_locator_t
        {
            /// <summary>
            /// For x64 is set to COL_SIG_REV1 which is 1.
            /// </summary>
            std::uint32_t signature;

            /// <summary>
            /// The offset from the complete object current sub-object from which we've taken complete_object_locator_t.
            /// </summary>
            std::uint32_t offset;

            /// <summary>
            /// The constructor displacement offset.
            /// </summary>
            std::uint32_t cd_offset;

            /// <summary>
            /// The offset from the image base to the type descriptor.
            /// </summary>
            std::int32_t type_descriptor_offset;

            /// <summary>
            /// The offset from the image base to the class_heirarchy_descriptor_t.
            /// </summary>
            std::int32_t class_descriptor_offset;

            /// <summary>
            /// The offset from the image base to the current object locator.
            /// </summary>
            std::int32_t self_offset;
        };
    }

    /// <summary>
    /// Represents an object (class/struct) in the module.
    /// </summary>
    struct module_t::object_t final
    {
        friend struct module_t;
    };

}  // namespace wincpp::modules