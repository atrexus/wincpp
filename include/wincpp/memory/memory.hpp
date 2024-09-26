#pragma once

// clang-format off
#include "wincpp/memory_factory.hpp"
// clang-format on

#include <Psapi.h>

namespace wincpp::patterns
{
    /// <summary>
    /// Forward declaration of the scanner class.
    /// </summary>
    class scanner;
}  // namespace wincpp::patterns

namespace wincpp::memory
{
    /// <summary>
    /// Contains extended working set information for a page.
    /// </summary>
    struct working_set_information_t
    {
        /// <summary>
        /// Creates a new working set information object.
        /// </summary>
        /// <param name="info">The working set information.</param>
        working_set_information_t( const PSAPI_WORKING_SET_EX_INFORMATION& info ) noexcept;

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
    /// An abstract structure representing a memory object. These object can read and write memory, allocate and free memory, and perform other memory
    /// operations.
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

        // This forces the structure to be abstract.
        virtual ~memory_t() = default;

        /// <summary>
        /// Gets the address of the memory object.
        /// </summary>
        constexpr std::uintptr_t address() const noexcept;

        /// <summary>
        /// Gets the size of the memory object.
        /// </summary>
        constexpr std::size_t size() const noexcept;

        /// <summary>
        /// Determines if the object contains the specified address.
        /// </summary>
        /// <param name="address">The address to check.</param>
        /// <returns>True if the region contains the address, false otherwise.</returns>
        constexpr bool contains( std::uintptr_t address ) const noexcept;

        /// <summary>
        /// Queries the working set information for the memory object. It will return information about the page that contains the address.
        /// </summary>
        working_set_information_t working_set_information() const;

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

        /// <summary>
        /// Gets the regions of the memory object.
        /// </summary>
        memory::region_list regions() const;

        /// <summary>
        /// Gets the scanner instance for the current memory object.
        /// </summary>
        patterns::scanner scanner() const;

        memory_factory factory;

       private:
        std::uintptr_t _address;
        std::size_t _size;
    };

    constexpr std::uintptr_t memory_t::address() const noexcept
    {
        return _address;
    }

    constexpr std::size_t memory_t::size() const noexcept
    {
        return _size;
    }

    constexpr bool memory_t::contains( std::uintptr_t address ) const noexcept
    {
        return _address <= address && address <= _address + _size;
    }

    inline std::shared_ptr< std::uint8_t[] > memory_t::read( std::uintptr_t address, std::size_t size ) const
    {
        return factory.read( address, size );
    }

    inline std::size_t memory_t::write( std::uintptr_t address, std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) const
    {
        return factory.write( address, buffer, size );
    }

    template< typename T >
    inline T memory_t::read( std::uintptr_t address ) const
    {
        return factory.read< T >( address );
    }

    template< typename T >
    inline void memory_t::write( std::uintptr_t address, T value ) const
    {
        factory.write< T >( address, value );
    }
}  // namespace wincpp::memory