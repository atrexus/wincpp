#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace wincpp::modules
{
    /// <summary>
    /// Forward declare the module_t structure.
    /// </summary>
    struct module_t;
}  // namespace wincpp::modules

/// <summary>
/// Contains all structures for MSVC RTTI.
/// </summary>
/// </summary>
namespace wincpp::modules::rtti
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
        std::string name;
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

    struct class_heirarchy_descriptor_t
    {
        /// <summary>
        /// The signature of the class heirarchy descriptor.
        /// </summary>
        std::uint32_t signature;

        /// <summary>
        /// The attributes of the class heirarchy descriptor.
        /// </summary>
        std::uint32_t attributes;

        /// <summary>
        /// The number of base classes.
        /// </summary>
        std::uint32_t num_base_classes;

        /// <summary>
        /// The image base relative offset to the base classes array.
        /// </summary>
        std::uintptr_t base_classes_offset;
    };

    /// <summary>
    /// Represents an object (class/struct) in the module.
    /// </summary>
    struct object_t final
    {
        friend struct module_t;

        /// <summary>
        /// Gets the type descriptor of the object. This is the RTTI information.
        /// </summary>
        rtti::type_descriptor_t type_descriptor() const noexcept;

        /// <summary>
        /// Gets the demangled name of the object.
        /// </summary>
        std::string name() const noexcept;

        /// <summary>
        /// Gets the address of the vtable.
        /// </summary>
        std::uintptr_t vtable() const noexcept;

       private:
        const module_t* mod;
        std::uintptr_t vtable_address;
        rtti::complete_object_locator_t col;

        /// <summary>
        /// Creates a new object object.
        /// </summary>
        /// <param name="module">The module object.</param>
        /// <param name="vtable_address">The address of the vtable.</param>
        /// <param name="col">The complete object locator.</param>
        explicit object_t( const module_t* module, std::uintptr_t vtable_address, const rtti::complete_object_locator_t& col ) noexcept;
    };

}  // namespace wincpp::modules::rtti