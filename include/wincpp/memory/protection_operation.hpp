#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include "wincpp/core/win.hpp"
#include "wincpp/memory/protection.hpp"

namespace wincpp
{
    class memory_factory;
}  // namespace wincpp

namespace wincpp::memory
{
    /// <summary>
    /// Represents an active memory protection operation.
    /// </summary>
    struct protection_operation_t final
    {
        friend class wincpp::memory_factory;

        /// <summary>
        /// The deleter for the protection operation object.
        /// </summary>
        struct deleter final
        {
            /// <summary>
            /// Creates a new protection operation deleter.
            /// </summary>
            /// <param name="handle">The process handle.</param>
            /// <param name="scoped">Whether the old protection should be restored.</param>
            deleter( std::shared_ptr< core::handle_t > handle, bool scoped ) noexcept;

            /// <summary>
            /// Deletes the protection operation object.
            /// </summary>
            /// <param name="operation">The protection operation.</param>
            void operator()( protection_operation_t* operation ) const;

           private:
            std::shared_ptr< core::handle_t > handle;
            bool scoped;
        };

       private:
        protection_flags_t new_flags;
        protection_flags_t old_flags;
        std::uintptr_t address;
        std::size_t size;

        /// <summary>
        /// Creates a new protection operation object.
        /// </summary>
        /// <param name="address">The address of the memory to protect.</param>
        /// <param name="size">The size of the memory to protect.</param>
        /// <param name="new_flags">The new protection flags.</param>
        /// <param name="old_flags">The old protection flags.</param>
        protection_operation_t( std::uintptr_t address, std::size_t size, protection_flags_t new_flags, protection_flags_t old_flags ) noexcept;
    };

    /// <summary>
    /// The protection operation owner.
    /// </summary>
    using protection_operation = std::unique_ptr< protection_operation_t, protection_operation_t::deleter >;
}  // namespace wincpp::memory

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/memory/protection_operation.inl"
#endif
