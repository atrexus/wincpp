#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <vector>

#include "wincpp/memory/protection.hpp"
#include "wincpp/memory/protection_operation.hpp"
#include "wincpp/patterns/scanner.hpp"

namespace wincpp
{
    class memory_factory;
}  // namespace wincpp

namespace wincpp::patterns
{
    /// <summary>
    /// Forward declaration of the pattern_t struct.
    /// </summary>
    struct pattern_t;
}  // namespace wincpp::patterns

namespace wincpp::memory
{
    /// <summary>
    /// The region_t struct.
    /// </summary>
    struct region_t;

    /// <summary>
    /// Represents a list of memory regions.
    /// </summary>
    class region_list;

    /// <summary>
    /// Contains extended working set information for a page.
    /// </summary>
    struct working_set_information_t
    {
        /// <summary>
        /// The virtual address of the page.
        /// </summary>
        std::uintptr_t virtual_address;

        /// <summary>
        /// If true, the page is in the working set list.
        /// </summary>
        bool valid;

        /// <summary>
        /// The number of processes that share the page.
        /// </summary>
        std::size_t share_count;

        /// <summary>
        /// The memory protection attributes of the page.
        /// </summary>
        protection_flags_t protection;
    };

    /// <summary>
    /// An abstract structure representing a memory object.
    /// </summary>
    struct memory_t
    {
        /// <summary>
        /// Creates a new memory object.
        /// </summary>
        /// <param name="mem">The process's memory factory.</param>
        /// <param name="address">The address of the memory object.</param>
        /// <param name="size">The size of the memory object.</param>
        explicit memory_t( const memory_factory& mem, std::uintptr_t address, std::size_t size ) noexcept;

        /// <summary>
        /// Function used to decide whether a memory region should be included in a memory search.
        /// </summary>
        using region_filter = std::function< bool( const memory::region_t& ) >;

        /// <summary>
        /// Destroys the memory object.
        /// </summary>
        virtual ~memory_t() = default;

        /// <summary>
        /// Gets the address of the memory object.
        /// </summary>
        /// <returns>The memory address.</returns>
        constexpr std::uintptr_t address() const noexcept;

        /// <summary>
        /// Gets the size of the memory object.
        /// </summary>
        /// <returns>The memory size.</returns>
        constexpr std::size_t size() const noexcept;

        /// <summary>
        /// Determines if the object contains the specified address.
        /// </summary>
        /// <param name="address">The address to check.</param>
        /// <returns>True if the region contains the address.</returns>
        constexpr bool contains( std::uintptr_t address ) const noexcept;

        /// <summary>
        /// Queries the working set information for the memory object.
        /// </summary>
        /// <returns>The working set information.</returns>
        working_set_information_t working_set_information() const;

        /// <summary>
        /// Reads the entire memory object.
        /// </summary>
        /// <returns>A shared pointer to the memory read.</returns>
        std::shared_ptr< std::uint8_t[] > read() const;

        /// <summary>
        /// Reads the memory object into the buffer.
        /// </summary>
        /// <param name="buffer">The buffer to read into.</param>
        void read( std::uint8_t* buffer ) const;

        /// <summary>
        /// Reads bytes from the memory object into a byte span.
        /// </summary>
        /// <param name="buffer">The buffer to read into.</param>
        void read( std::span< std::byte > buffer ) const;

        /// <summary>
        /// Reads memory from the process.
        /// </summary>
        /// <param name="offset">The offset to read from.</param>
        /// <param name="size">The size of the memory to read.</param>
        /// <returns>The memory read.</returns>
        std::shared_ptr< std::uint8_t[] > read( std::uintptr_t offset, std::size_t size ) const;

        /// <summary>
        /// Reads bytes from the memory object into a byte span.
        /// </summary>
        /// <param name="offset">The offset to read from.</param>
        /// <param name="buffer">The buffer to read into.</param>
        void read( std::uintptr_t offset, std::span< std::byte > buffer ) const;

        /// <summary>
        /// Reads a value from memory.
        /// </summary>
        /// <typeparam name="T">The type of value to read.</typeparam>
        /// <param name="offset">The offset to read from.</param>
        /// <returns>The value read.</returns>
        template< typename T >
            requires std::is_trivially_copyable_v< T >
        T read( std::uintptr_t offset ) const;

        /// <summary>
        /// Writes memory to the process.
        /// </summary>
        /// <param name="offset">The offset to write to.</param>
        /// <param name="buffer">The buffer to write.</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes written.</returns>
        std::size_t write( std::uintptr_t offset, const std::uint8_t* buffer, std::size_t size ) const;

        /// <summary>
        /// Writes memory to the process.
        /// </summary>
        /// <param name="offset">The offset to write to.</param>
        /// <param name="buffer">The buffer to write.</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes written.</returns>
        std::size_t write( std::uintptr_t offset, const std::shared_ptr< std::uint8_t[] >& buffer, std::size_t size ) const;

        /// <summary>
        /// Writes a byte span to the memory object.
        /// </summary>
        /// <param name="buffer">The buffer to write.</param>
        /// <returns>The number of bytes written.</returns>
        std::size_t write( std::span< const std::byte > buffer ) const;

        /// <summary>
        /// Writes a byte span to the memory object.
        /// </summary>
        /// <param name="offset">The offset to write to.</param>
        /// <param name="buffer">The buffer to write.</param>
        /// <returns>The number of bytes written.</returns>
        std::size_t write( std::uintptr_t offset, std::span< const std::byte > buffer ) const;

        /// <summary>
        /// Writes a value to memory.
        /// </summary>
        /// <typeparam name="T">The type of value to write.</typeparam>
        /// <param name="offset">The offset to write to.</param>
        /// <param name="value">The value to write.</param>
        template< typename T >
            requires std::is_trivially_copyable_v< T >
        void write( std::uintptr_t offset, const T& value ) const;

        /// <summary>
        /// Gets the regions of the memory object.
        /// </summary>
        /// <returns>The memory region list.</returns>
        memory::region_list regions() const;

        /// <summary>
        /// Searches for the pattern in the memory object.
        /// </summary>
        /// <param name="pattern">The pattern to search for.</param>
        /// <returns>The relative location.</returns>
        std::optional< std::uintptr_t > find( const patterns::pattern_t& pattern ) const noexcept;

        /// <summary>
        /// Searches for the pattern in the memory object with the specified scanner algorithm.
        /// </summary>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The scanner algorithm to use.</param>
        /// <returns>The relative location.</returns>
        std::optional< std::uintptr_t > find( const patterns::pattern_t& pattern, patterns::scanner::algorithm_t algorithm ) const;

        /// <summary>
        /// Searches for the pattern in the memory object with the specified scanner algorithm and region filter.
        /// </summary>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The scanner algorithm to use.</param>
        /// <param name="filter">The region filter to apply before scanning.</param>
        /// <returns>The relative location.</returns>
        std::optional< std::uintptr_t >
        find( const patterns::pattern_t& pattern, patterns::scanner::algorithm_t algorithm, const region_filter& filter ) const;

        /// <summary>
        /// Searches for all occurrences of the pattern in the memory object.
        /// </summary>
        /// <param name="pattern">The pattern to search for.</param>
        /// <returns>The relative locations.</returns>
        std::vector< std::uintptr_t > find_all( const patterns::pattern_t& pattern ) const noexcept;

        /// <summary>
        /// Searches for all occurrences of the pattern in the memory object with the specified scanner algorithm.
        /// </summary>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The scanner algorithm to use.</param>
        /// <returns>The relative locations.</returns>
        std::vector< std::uintptr_t > find_all( const patterns::pattern_t& pattern, patterns::scanner::algorithm_t algorithm ) const;

        /// <summary>
        /// Searches for all occurrences of the pattern in the memory object with the specified scanner algorithm and region filter.
        /// </summary>
        /// <param name="pattern">The pattern to search for.</param>
        /// <param name="algorithm">The scanner algorithm to use.</param>
        /// <param name="filter">The region filter to apply before scanning.</param>
        /// <returns>The relative locations.</returns>
        std::vector< std::uintptr_t >
        find_all( const patterns::pattern_t& pattern, patterns::scanner::algorithm_t algorithm, const region_filter& filter ) const;

        /// <summary>
        /// Changes the protection of the memory region.
        /// </summary>
        /// <param name="offset">The offset of the memory region.</param>
        /// <param name="size">The size of the memory region.</param>
        /// <param name="new_flags">The new protection flags.</param>
        /// <param name="scoped">Whether the protection operation is scoped.</param>
        /// <returns>A protection operation object.</returns>
        protection_operation protect( std::uintptr_t offset, std::size_t size, protection_flags_t new_flags, bool scoped ) const;

        /// <summary>
        /// Changes the protection of the memory region.
        /// </summary>
        /// <param name="new_flags">The new protection flags.</param>
        /// <param name="scoped">Whether the protection operation is scoped.</param>
        /// <returns>The protection operation object.</returns>
        protection_operation protect( protection_flags_t new_flags, bool scoped ) const;

        /// <summary>
        /// Gets the memory factory used by this memory object.
        /// </summary>
        /// <returns>The memory factory.</returns>
        const memory_factory& memory() const noexcept;

       protected:
        const memory_factory* factory;

       private:
        bool is_valid_region( const memory::region_t& region ) const noexcept;
        std::uintptr_t checked_address( std::uintptr_t offset, std::size_t size ) const;

        std::uintptr_t _address;
        std::size_t _size;
    };
}  // namespace wincpp::memory

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/memory/memory.inl"
#endif
