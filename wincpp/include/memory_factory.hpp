#pragma once

#include "memory/pointer.hpp"
#include "memory/region.hpp"

namespace wincpp::memory
{
    template< typename T >
    struct pointer_t;
}

namespace wincpp
{
    /// <summary>
    /// Defines the types of memory manipulations.
    /// </summary>
    enum class memory_type
    {
        /// <summary>
        /// The memory is within the local process. Often, this is called "injected" or "Internal".
        /// </summary>
        local_t,

        /// <summary>
        /// The memory is not within the local process. Often this is called "remote" or "external".
        /// </summary>
        remote_t
    };

    /// <summary>
    /// Class providing tools for manipulating memory.
    /// </summary>
    class memory_factory final
    {
        friend class process_t;

        constexpr static std::size_t buffer_size = 256;

        process_t *p;
        memory_type type;

        /// <summary>
        /// Creates a new memory factory object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="type">The memory type.</param>
        explicit memory_factory( process_t *p, memory_type type ) noexcept;

       public:
        /// <summary>
        /// Reads memory from the process.
        /// </summary>
        /// <param name="address">The address to read from.</param>
        /// <param name="size">The size of the memory to read.</param>
        /// <returns>The memory read.</returns>
        std::shared_ptr< std::uint8_t[] > read( std::uintptr_t address, std::size_t size ) const;

        /// <summary>
        /// Reads a value from memory.
        /// </summary>
        /// <typeparam name="T">The type of value to read.</typeparam>
        /// <param name="address">The address to read from.</param>
        /// <returns>The value read.</returns>
        template< typename T >
        T read( std::uintptr_t address ) const;

        /// <summary>
        /// Writes memory to the process.
        /// </summary>
        /// <param name="address">The address to write to.</param>
        /// <param name="buffer">The buffer to write.</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes written.</returns>
        std::size_t write( std::uintptr_t address, std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) const;

        /// <summary>
        /// Writes a value to memory.
        /// </summary>
        /// <typeparam name="T">The type of value to write.</typeparam>
        /// <param name="address">The address to write to.</param>
        /// <param name="value">The value to write.</param>
        template< typename T >
        void write( std::uintptr_t address, T value ) const;

        /// <summary>
        /// Gets a pointer to the memory.
        /// </summary>
        /// <param name="address">The address of the pointer.</param>
        /// <returns>The pointer to the memory.</returns>
        const memory::pointer_t< std::uintptr_t > &operator[]( std::uintptr_t address ) const;

        /// <summary>
        /// Gets all regions in the process.
        /// </summary>
        /// <param name="start">The address to start at.</param>
        /// <param name="stop">The address to stop at.</param>
        /// <returns>The region list.</returns>
        memory::region_list regions( std::uintptr_t start = 0, std::uintptr_t stop = -1 ) const;
    };

    template< typename T >
    inline T memory_factory::read( std::uintptr_t address ) const
    {
        return *reinterpret_cast< T * >( read( address, sizeof( T ) ).get() );
    }

    template<>
    inline std::string memory_factory::read< std::string >( std::uintptr_t address ) const
    {
        return std::string( reinterpret_cast< const char * >( read( address, buffer_size ).get() ) );
    }

    template< typename T >
    inline void memory_factory::write( std::uintptr_t address, T value ) const
    {
        // Allocate a buffer for the value.
        auto buffer = std::shared_ptr< std::uint8_t[] >( new std::uint8_t[ sizeof( T ) ] );

        // Copy the value into the buffer.
        std::memcpy( buffer.get(), &value, sizeof( T ) );

        // Write the buffer to the process.
        write( address, buffer, sizeof( T ) );
    }

    template<>
    inline void memory_factory::write< std::string >( std::uintptr_t address, std::string value ) const
    {
        // Allocate a buffer for the value.
        auto buffer = std::shared_ptr< std::uint8_t[] >( new std::uint8_t[ value.size() + 1 ] );

        // Copy the value into the buffer.
        std::memcpy( buffer.get(), value.c_str(), value.size() + 1 );

        // Write the buffer to the process.
        write( address, buffer, value.size() + 1 );
    }

}  // namespace wincpp