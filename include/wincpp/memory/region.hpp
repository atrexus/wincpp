#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include "wincpp/memory/memory.hpp"

namespace wincpp
{
    struct process_t;
}  // namespace wincpp

namespace wincpp::memory
{
    /// <summary>
    /// Represents a contiguous block of memory in the remote process.
    /// </summary>
    struct region_t : public memory_t
    {
        friend class region_list;

        /// <summary>
        /// The state of the memory region.
        /// </summary>
        enum class state_t : std::uint32_t
        {
            /// <summary>
            /// Indicates committed pages for which physical storage has been allocated.
            /// </summary>
            commit_t = 0x00001000,

            /// <summary>
            /// Indicates reserved pages.
            /// </summary>
            reserve_t = 0x00002000,

            /// <summary>
            /// Indicates free pages.
            /// </summary>
            free_t = 0x00010000
        };

        /// <summary>
        /// The type of pages in the region.
        /// </summary>
        enum class type_t : std::uint32_t
        {
            /// <summary>
            /// Indicates that the memory pages within the region are mapped into an image section.
            /// </summary>
            image_t = 0x01000000,

            /// <summary>
            /// Indicates that the memory pages within the region are mapped into a section.
            /// </summary>
            mapped_t = 0x00040000,

            /// <summary>
            /// Indicates that the memory pages within the region are private.
            /// </summary>
            private_t = 0x00020000
        };

        /// <summary>
        /// Gets the state of the pages in the region.
        /// </summary>
        /// <returns>The state of the region.</returns>
        state_t state() const noexcept;

        /// <summary>
        /// Gets the type of the pages in the region.
        /// </summary>
        /// <returns>The type of the region.</returns>
        type_t type() const noexcept;

        /// <summary>
        /// Gets the protection of the pages in the region.
        /// </summary>
        /// <returns>The protection flags.</returns>
        protection_flags_t protection() const noexcept;

        /// <summary>
        /// Creates a new region object.
        /// </summary>
        /// <param name="factory">The memory factory.</param>
        /// <param name="address">The base address.</param>
        /// <param name="size">The region size.</param>
        /// <param name="state">The memory state.</param>
        /// <param name="type">The memory type.</param>
        /// <param name="protection">The memory protection.</param>
        explicit region_t(
            const memory_factory& factory,
            std::uintptr_t address,
            std::size_t size,
            state_t state,
            type_t type,
            protection_flags_t protection ) noexcept;

       private:
        state_t _state;
        type_t _type;
        protection_flags_t _protection;
    };

    /// <summary>
    /// Represents a list of memory regions in the remote process.
    /// </summary>
    class region_list final
    {
        friend class wincpp::memory_factory;

        process_t* process;
        std::uintptr_t start;
        std::uintptr_t stop;

        /// <summary>
        /// Creates a new region list object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="start">The start address.</param>
        /// <param name="stop">The stop address.</param>
        explicit region_list( process_t* process, std::uintptr_t start = 0, std::uintptr_t stop = static_cast< std::uintptr_t >( -1 ) ) noexcept;

       public:
        /// <summary>
        /// The iterator for the region list.
        /// </summary>
        class iterator;

        /// <summary>
        /// Gets the begin iterator for the region list.
        /// </summary>
        /// <returns>The begin iterator.</returns>
        iterator begin() const;

        /// <summary>
        /// Gets the end iterator for the region list.
        /// </summary>
        /// <returns>The end iterator.</returns>
        iterator end() const noexcept;
    };

    /// <summary>
    /// Represents an iterator for the region list.
    /// </summary>
    class region_list::iterator
    {
        friend class region_list;

        process_t* process;
        std::uintptr_t address;
        std::uintptr_t stop;
        std::optional< region_t > current;

        /// <summary>
        /// Creates a new iterator object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="address">The address of the region.</param>
        /// <param name="stop">The stop address.</param>
        explicit iterator( process_t* process, std::uintptr_t address, std::uintptr_t stop );

       public:
        /// <summary>
        /// Gets the current region.
        /// </summary>
        /// <returns>The current region.</returns>
        region_t operator*() const;

        /// <summary>
        /// Advances the iterator to the next region.
        /// </summary>
        /// <returns>The iterator.</returns>
        iterator& operator++();

        /// <summary>
        /// Compares two iterators for equality.
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>True if the iterators are equal.</returns>
        bool operator==( const iterator& other ) const noexcept;

        /// <summary>
        /// Compares two iterators for inequality.
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>True if the iterators are not equal.</returns>
        bool operator!=( const iterator& other ) const noexcept;

       private:
        void read_current();
    };
}  // namespace wincpp::memory

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/memory/region.inl"
#endif
