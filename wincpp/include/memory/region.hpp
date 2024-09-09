#pragma once

#include "process.hpp"
#include "protection.hpp"

namespace wincpp
{
    class memory_factory;
}  // namespace wincpp

namespace wincpp::memory
{
    class region_list;

    /// <summary>
    /// Represents a contiguous block of memory in the remote process.
    /// </summary>
    struct region_t final
    {
        friend class region_list;

        /// <summary>
        /// The state of the memory region.
        /// </summary>
        enum class state_t
        {
            /// <summary>
            /// Indicates committed pages for which physical storage has been allocated, either in memory or in the paging file on disk.
            /// </summary>
            commit_t = MEM_COMMIT,

            /// <summary>
            /// Indicates reserved pages where a range of the process's virtual address space is reserved without any physical storage being
            /// allocated.
            /// </summary>
            reserve_t = MEM_RESERVE,

            /// <summary>
            /// Indicates free pages not accessible to the calling process and available to be allocated.
            /// </summary>
            free_t = MEM_FREE
        };

        /// <summary>
        /// The type of pages in the region.
        /// </summary>
        enum class type_t
        {
            /// <summary>
            /// Indicates that the memory pages within the region are mapped into the view of an image section.
            /// </summary>
            image_t = MEM_IMAGE,

            /// <summary>
            /// Indicates that the memory pages within the region are mapped into the view of a section.
            /// </summary>
            mapped_t = MEM_MAPPED,

            /// <summary>
            /// Indicates that the memory pages within the region are private (that is, not shared by other processes).
            /// </summary>
            private_t = MEM_PRIVATE
        };

        /// <summary>
        /// Gets the base address of the region.
        /// </summary>
        std::uintptr_t address() const noexcept;

        /// <summary>
        /// Gets the state of the pages in the region.
        /// </summary>
        state_t state() const noexcept;

        /// <summary>
        /// Gets the type of the pages in the region.
        /// </summary>
        type_t type() const noexcept;

        /// <summary>
        /// Gets the protection of the pages in the region.
        /// </summary>
        protection_flags_t protection() const noexcept;

        /// <summary>
        /// Gets the size of the region.
        /// </summary>
        std::size_t size() const noexcept;

        /// <summary>
        /// Determines if the region contains the specified address.
        /// </summary>
        /// <param name="address">The address to check.</param>
        /// <returns>True if the region contains the address, false otherwise.</returns>
        inline bool contains( std::uintptr_t address ) const noexcept;

        /// <summary>
        /// Reads memory from the process.
        /// </summary>
        /// <param name="address">The address to read from.</param>
        /// <param name="size">The size of the memory to read.</param>
        /// <returns>The memory read.</returns>
        inline std::shared_ptr< std::uint8_t[] > read( std::uintptr_t address, std::size_t size ) const;

        /// <summary>
        /// Reads a value from memory.
        /// </summary>
        /// <typeparam name="T">The type of value to read.</typeparam>
        /// <param name="address">The address to read from.</param>
        /// <returns>The value read.</returns>
        template< typename T >
        inline T read( std::uintptr_t address ) const;

        /// <summary>
        /// Writes memory to the process.
        /// </summary>
        /// <param name="address">The address to write to.</param>
        /// <param name="buffer">The buffer to write.</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes written.</returns>
        inline std::size_t write( std::uintptr_t address, std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) const;

        /// <summary>
        /// Writes a value to memory.
        /// </summary>
        /// <typeparam name="T">The type of value to write.</typeparam>
        /// <param name="address">The address to write to.</param>
        /// <param name="value">The value to write.</param>
        template< typename T >
        inline void write( std::uintptr_t address, T value ) const;

       private:
        /// <summary>
        /// Creates a new region object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="mbi">The memory basic information.</param>
        explicit region_t( process_t *process, MEMORY_BASIC_INFORMATION mbi );

        process_t *process;
        MEMORY_BASIC_INFORMATION mbi;
    };

    /// <summary>
    /// Represents a list of memory regions in the remote process. Contains the iterator for VirtualQueryEx.
    /// </summary>
    class region_list final
    {
        friend class memory_factory;

        process_t *process;
        std::uintptr_t start, stop;

        /// <summary>
        /// Creates a new region list object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="address">The address to start at.</param>
        /// <param name="stop">The address to stop at.</param>
        explicit region_list( process_t *process, std::uintptr_t start = 0, std::uintptr_t stop = -1 ) noexcept;

       public:
        /// <summary>
        /// The iterator for the region list.
        /// </summary>
        class iterator;

        /// <summary>
        /// Gets the begin iterator for the region list.
        /// </summary>
        iterator begin() const noexcept;

        /// <summary>
        /// Gets the end iterator for the region list.
        /// </summary>
        iterator end() const noexcept;
    };

    /// <summary>
    /// Represents an iterator for the region list.
    /// </summary>
    class region_list::iterator
    {
        friend class region_list;

        MEMORY_BASIC_INFORMATION mbi;
        process_t *process;
        std::uintptr_t address;

        /// <summary>
        /// Creates a new iterator object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="address">The address of the region.</param>
        explicit iterator( process_t *process, std::uintptr_t address );

       public:
        /// <summary>
        /// Gets the current region.
        /// </summary>
        region_t operator*() const noexcept;

        /// <summary>
        /// Advances the iterator to the next region.
        /// </summary>
        iterator &operator++() noexcept;

        /// <summary>
        /// Compares two iterators for equality.
        /// </summary>
        bool operator==( const iterator &other ) const noexcept;

        /// <summary>
        /// Compares two iterators for inequality.
        /// </summary>
        bool operator!=( const iterator &other ) const noexcept;
    };

    template< typename T >
    inline T region_t::read( std::uintptr_t address ) const
    {
        return process->memory_factory.read< T >( address );
    }

    template< typename T >
    inline void region_t::write( std::uintptr_t address, T value ) const
    {
        process->memory_factory.write( address, value );
    }

}  // namespace wincpp::memory