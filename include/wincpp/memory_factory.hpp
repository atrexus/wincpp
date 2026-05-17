#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>

#include "wincpp/memory/protection_operation.hpp"
#include "wincpp/modules/object.hpp"

#ifndef WINCPP_SUPPRESS_AUTO_INL
#define WINCPP_SUPPRESS_AUTO_INL
#define WINCPP_RESTORE_REGION_AUTO_INL
#endif

#include "wincpp/memory/region.hpp"

#ifdef WINCPP_RESTORE_REGION_AUTO_INL
#undef WINCPP_SUPPRESS_AUTO_INL
#undef WINCPP_RESTORE_REGION_AUTO_INL
#endif

namespace wincpp::memory
{
    /// <summary>
    /// Forward declare the memory_t struct.
    /// </summary>
    struct memory_t;

    /// <summary>
    /// Forward declare the pointer_t struct.
    /// </summary>
    /// <typeparam name="T">The pointer value type.</typeparam>
    template< typename T >
    struct pointer_t;

    /// <summary>
    /// Forward declare the region_list class.
    /// </summary>
    class region_list;

    /// <summary>
    /// Forward declare the region_t struct.
    /// </summary>
    struct region_t;

    /// <summary>
    /// Forward declare the working_set_information_t struct.
    /// </summary>
    struct working_set_information_t;

    /// <summary>
    /// Forward declare the allocation_t struct.
    /// </summary>
    struct allocation_t;
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
    enum class memory_type : std::uint8_t
    {
        /// <summary>
        /// The memory is within the local process.
        /// </summary>
        local_t,

        /// <summary>
        /// The memory is not within the local process.
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
        friend struct memory::memory_t;

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
        /// Reads memory from the process into a user-provided buffer.
        /// </summary>
        /// <param name="address">The address to read from.</param>
        /// <param name="size">The size of the memory to read.</param>
        /// <param name="buffer">The buffer to read into.</param>
        /// <returns>True if the full buffer was read.</returns>
        bool read( std::uintptr_t address, std::size_t size, std::uint8_t* buffer ) const noexcept;

        /// <summary>
        /// Reads memory from the process into a byte span.
        /// </summary>
        /// <param name="address">The address to read from.</param>
        /// <param name="buffer">The buffer to read into.</param>
        /// <returns>True if the full buffer was read.</returns>
        bool read( std::uintptr_t address, std::span< std::byte > buffer ) const noexcept;

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
            requires std::is_trivially_copyable_v< T >
        T read( std::uintptr_t address ) const;

        /// <summary>
        /// Reads a string from memory.
        /// </summary>
        /// <param name="address">The address to read from.</param>
        /// <returns>The string read.</returns>
        std::string read_string( std::uintptr_t address ) const;

        /// <summary>
        /// Reads a null-terminated narrow string from memory with an explicit maximum length.
        /// </summary>
        /// <param name="address">The address to read from.</param>
        /// <param name="max_length">The maximum number of characters to read, including any null terminator.</param>
        /// <returns>The string read.</returns>
        std::string read_string( std::uintptr_t address, std::size_t max_length ) const;

        /// <summary>
        /// Reads a null-terminated wide string from memory with an explicit maximum length.
        /// </summary>
        /// <param name="address">The address to read from.</param>
        /// <param name="max_length">The maximum number of wide characters to read, including any null terminator.</param>
        /// <returns>The wide string read.</returns>
        std::wstring read_wstring( std::uintptr_t address, std::size_t max_length ) const;

        /// <summary>
        /// Writes memory to the process.
        /// </summary>
        /// <param name="address">The address to write to.</param>
        /// <param name="buffer">The buffer to write.</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes written.</returns>
        std::size_t write( std::uintptr_t address, const std::shared_ptr< std::uint8_t[] >& buffer, std::size_t size ) const noexcept;

        /// <summary>
        /// Writes memory to the process.
        /// </summary>
        /// <param name="address">The address to write to.</param>
        /// <param name="buffer">The buffer to write.</param>
        /// <param name="size">The size of the buffer.</param>
        /// <returns>The number of bytes written.</returns>
        std::size_t write( std::uintptr_t address, const std::uint8_t* buffer, std::size_t size ) const noexcept;

        /// <summary>
        /// Writes a byte span to the process.
        /// </summary>
        /// <param name="address">The address to write to.</param>
        /// <param name="buffer">The buffer to write.</param>
        /// <returns>The number of bytes written.</returns>
        std::size_t write( std::uintptr_t address, std::span< const std::byte > buffer ) const noexcept;

        /// <summary>
        /// Writes a value to memory.
        /// </summary>
        /// <typeparam name="T">The type of value to write.</typeparam>
        /// <param name="address">The address to write to.</param>
        /// <param name="value">The value to write.</param>
        template< typename T >
            requires std::is_trivially_copyable_v< T >
        void write( std::uintptr_t address, const T& value ) const;

        /// <summary>
        /// Writes a string to memory.
        /// </summary>
        /// <param name="address">The address to write to.</param>
        /// <param name="value">The value to write.</param>
        void write_string( std::uintptr_t address, std::string_view value ) const;

        /// <summary>
        /// Writes a null-terminated wide string to memory.
        /// </summary>
        /// <param name="address">The address to write to.</param>
        /// <param name="value">The wide string value to write.</param>
        void write_wstring( std::uintptr_t address, std::wstring_view value ) const;

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
        memory::region_list regions( std::uintptr_t start = 0, std::uintptr_t stop = static_cast< std::uintptr_t >( -1 ) ) const;

        /// <summary>
        /// Queries the memory region that contains the specified address.
        /// </summary>
        /// <param name="address">The address to query.</param>
        /// <returns>The region containing the address, if the query succeeded.</returns>
        std::optional< memory::region_t > query( std::uintptr_t address ) const noexcept;

        /// <summary>
        /// Changes the protection of the specified memory region.
        /// </summary>
        /// <param name="address">The address of the memory to protect.</param>
        /// <param name="size">The size of the memory to protect.</param>
        /// <param name="new_flags">The new protection flags.</param>
        /// <param name="scoped">Whether the protection operation is scoped.</param>
        /// <returns>The protection operation.</returns>
        memory::protection_operation protect( std::uintptr_t address, std::size_t size, memory::protection_flags_t new_flags, bool scoped = true )
            const;

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
        /// <param name="compare">An optional comparison function.</param>
        /// <param name="parallelize">Whether to use multiple threads to search.</param>
        /// <returns>The address of the object.</returns>
        std::optional< std::uintptr_t >
        find_instance_of( const std::shared_ptr< modules::rtti::object_t >& object, const region_compare& compare, bool parallelize = false ) const;

        /// <summary>
        /// Frees the memory at the specified address.
        /// </summary>
        /// <param name="address">The address of the memory to free.</param>
        void free( std::uintptr_t address ) const;

        /// <summary>
        /// Creates a new allocation in the process.
        /// </summary>
        /// <param name="size">The size of the allocation.</param>
        /// <param name="protection">The protection flags of the allocation.</param>
        /// <param name="owns">Whether the allocation owns the memory.</param>
        /// <returns>A shared pointer to the allocation.</returns>
        std::shared_ptr< memory::allocation_t > allocate( std::size_t size, memory::protection_flags_t protection, bool owns = true ) const;

        /// <summary>
        /// Creates a new allocation for the specified type.
        /// </summary>
        /// <typeparam name="T">The type to allocate.</typeparam>
        /// <param name="protection">The protection flags of the allocation.</param>
        /// <param name="owns">Whether the allocation owns the memory.</param>
        /// <returns>A shared pointer to the allocation.</returns>
        template< typename T >
        std::shared_ptr< memory::allocation_t > allocate( memory::protection_flags_t protection, bool owns = true ) const;
    };
}  // namespace wincpp

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/memory/region.inl"
#include "wincpp/memory_factory.inl"
#endif
