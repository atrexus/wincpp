#pragma once

#include <memory>

#include "protection.hpp"

namespace wincpp
{
    struct process_t;
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
        struct protection_operation_deleter final
        {
            /// <summary>
            /// Deletes the protection operation object.
            /// <summary>
            void operator()( protection_operation_t *operation ) const;
        };

       private:
        process_t *p;
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
        protection_operation_t(
            process_t *p,
            std::uintptr_t address,
            std::size_t size,
            protection_flags_t new_flags,
            protection_flags_t old_flags ) noexcept;
    };

    using protection_operation = std::unique_ptr< protection_operation_t, protection_operation_t::protection_operation_deleter >;

}  // namespace wincpp::memory