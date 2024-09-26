#pragma once

#include <memory>

#include "wincpp/core/win.hpp"
#include "protection.hpp"

namespace wincpp
{
    class memory_factory;
}  // namespace wincpp

namespace wincpp::memory
{
    struct protection_operation_t final
    {
        friend class memory_factory;

        /// <summary>
        /// The deleter for the protection operation object.
        /// </summary>
        struct deleter final
        {
            deleter( std::shared_ptr< core::handle_t > handle ) noexcept;

            /// <summary>
            /// Deletes the protection operation object.
            /// <summary>
            void operator()( protection_operation_t *operation ) const;

           private:
            std::shared_ptr< core::handle_t > handle;
        };

       private:
        protection_flags_t new_flags;
        protection_flags_t old_flags;
        std::uintptr_t address;
        std::size_t size;

        /// <summary>
        /// Creates a new protection operation object.
        /// </summary>
        /// <param name="p">The process object.</param>
        /// <param name="address">The address of the memory to protect.</param>
        /// <param name="size">The size of the memory to protect.</param>
        /// <param name="new_flags">The new protection flags.</param>
        /// <param name="old_flags">The old protection flags.</param>
        protection_operation_t( std::uintptr_t address, std::size_t size, protection_flags_t new_flags, protection_flags_t old_flags ) noexcept;
    };

    using protection_operation = std::unique_ptr< protection_operation_t, protection_operation_t::deleter >;

}  // namespace wincpp::memory