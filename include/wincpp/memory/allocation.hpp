#pragma once

#include "wincpp/memory/memory.hpp"

namespace wincpp::memory
{
    /// <summary>
    /// Represents an allocation in the process.
    /// </summary>
    struct allocation_t : memory_t
    {
        friend class wincpp::memory_factory;

        /// <summary>
        /// The deleter for the allocation.
        /// </summary>
        struct deleter
        {
            /// <summary>
            /// Frees the memory backing the allocation.
            /// </summary>
            /// <param name="allocation">The allocation to free.</param>
            void operator()( allocation_t* allocation ) const noexcept;
        };

       protected:
        /// <summary>
        /// Creates a new allocation.
        /// </summary>
        /// <param name="mem">The memory factory.</param>
        /// <param name="address">The address of the allocation.</param>
        /// <param name="size">The size of the allocation.</param>
        /// <param name="owns">Whether the allocation owns the backing memory.</param>
        explicit allocation_t( const memory_factory& mem, std::uintptr_t address, std::size_t size, bool owns = false ) noexcept;

        bool _owns;
    };
}  // namespace wincpp::memory

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/memory/allocation.inl"
#endif
