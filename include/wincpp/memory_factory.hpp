#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "memory/protection_operation.hpp"
#include "modules/object.hpp"

namespace wincpp::memory
{
    /// <summary>
    /// Forward declare the pointer_t struct.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    template< typename T >
    struct pointer_t;

    /// <summary>
    /// Forward declare the region_list class.
    /// </summary>
    class region_list;

    struct region_t;

    /// <summary>
    /// Forward declare the working_set_information_t struct.
    /// </summary>
    struct working_set_information_t;
}  // namespace wincpp::memory

namespace wincpp::modules
{
    /// <summary>
    /// Forward declare the module_t struct.
    /// </summary>
    struct module_t;
}  // namespace wincpp::modules

namespace wincpp
{
    struct process_t;

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
        friend struct process_t;
        friend struct modules::module_t;

        constexpr static std::size_t buffer_size = 256;

        process_t* p;
        memory_type type;

        /// <summary>
        /// Creates a new memory factory object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="type">The memory type.</param>
        explicit memory_factory( process_t* p, memory_type type ) noexcept;

       public:
        /// <summary>
        /// The region compare function. Its used to determine if a region should be searched or used.
        /// </summary>
        using region_compare = std::function< bool( const memory::region_t& ) >;

        /// <summary>
        /// Reads memory from the process.
        /// </summary>
        /// <param name="address">The address to read from.</param>
        /// <param name="size">The size of the memory to read.</param>
        /// <returns>The memory read.</returns>
        std::shared_ptr< std::uint8_t[] > read( std::uintptr_t address, std::size_t size ) const noexcept;

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
        memory::pointer_t< std::uintptr_t > operator[]( std::uintptr_t address ) const;

        /// <summary>
        /// Gets all regions in the process.
        /// </summary>
        /// <param name="start">The address to start at.</param>
        /// <param name="stop">The address to stop at.</param>
        /// <returns>The region list.</returns>
        memory::region_list regions( std::uintptr_t start = 0, std::uintptr_t stop = -1 ) const;

        /// <summary>
        /// Changes the protection of the specified memory region.
        /// </summary>
        /// <param name="address">The address of the memory to protect.</param>
        /// <param name="size">The size of the memory to protect.</param>
        /// <param name="new_flags">The new protection flags.</param>
        memory::protection_operation protect( std::uintptr_t address, std::size_t size, memory::protection_flags_t new_flags ) const;

        /// <summary>
        /// Gets the working set information for the specified address.
        /// </summary>
        /// <param name="address">The address to get the working set information for.</param>
        /// <returns>The working set information.</returns>
        memory::working_set_information_t working_set_information( std::uintptr_t address ) const;

        /// <summary>
        /// Find the first instance of the provided object in memory.
        /// </summary>
        /// <param name="object">The object to search for.</param>
        /// <param name="parallelize">Whether to use multiple threads to search.</param>
        /// <returns>The address of the object.</returns>
        std::optional< std::uintptr_t > find_instance_of( const std::shared_ptr< modules::rtti::object_t >& object, bool parallelize = false ) const;

        /// <summary>
        /// Find the first instance of the provided object in memory.
        /// </summary>
        /// <param name="object">The object to search for.</param>
        /// <param name="compare">An optional comparison function. If the region already matches the default criteria and `compare` returns true, the
        /// region is searched.</param>
        /// <param name="parallelize">Whether to use multiple threads to search.</param>
        /// <returns>The address of the object.</returns>
        std::optional< std::uintptr_t >
        find_instance_of( const std::shared_ptr< modules::rtti::object_t >& object, const region_compare& compare, bool parallelize = false ) const;
    };

    template< typename T >
    inline T memory_factory::read( std::uintptr_t address ) const
    {
        return *reinterpret_cast< T* >( read( address, sizeof( T ) ).get() );
    }

    template<>
    inline std::string memory_factory::read< std::string >( std::uintptr_t address ) const
    {
        return std::string( reinterpret_cast< const char* >( read( address, buffer_size ).get() ) );
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