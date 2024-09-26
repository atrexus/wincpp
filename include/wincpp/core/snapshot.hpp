#pragma once

// clang-format off
#include "win.hpp"
#include "error.hpp"
// clang-format on

#include <TlHelp32.h>

namespace wincpp::core
{
    /// <summary>
    /// The kind of snapshot we can take.
    /// </summary>
    enum class snapshot_kind : std::uint32_t
    {
        /// <summary>
        /// Includes all heaps of the process specified in th32ProcessID in the snapshot.
        /// </summary>
        heaplist_t = TH32CS_SNAPHEAPLIST,

        /// <summary>
        /// Includes all modules of the process specified in th32ProcessID in the snapshot.
        /// </summary>
        module_t = TH32CS_SNAPMODULE,

        /// <summary>
        /// Includes all 32-bit modules of the process specified in th32ProcessID in the snapshot when called from a 64-bit
        /// process.
        /// </summary>
        module32_t = TH32CS_SNAPMODULE32,

        /// <summary>
        /// Includes all processes in the system in the snapshot.
        /// </summary>
        process_t = TH32CS_SNAPPROCESS,

        /// <summary>
        /// Includes all threads in the system in the snapshot.
        /// </summary>
        thread_t = TH32CS_SNAPTHREAD
    };

    /// <summary>
    /// Represents a basic snapshot created by the tool help library.
    /// </summary>
    template< snapshot_kind T >
    class snapshot final
    {
        std::shared_ptr< handle_t > handle;

       public:
        /// <summary>
        /// The iterator class (implementation varies).
        /// </summary>
        class iterator;

        /// <summary>
        /// Creates a new snapshot using an optional process identifier.
        /// </summary>
        /// <param name="id">The identifier.</param>
        /// <returns>The snapshot</returns>
        static snapshot< T > create( std::uint32_t id = 0 )
        {
            HANDLE hSnapshot = CreateToolhelp32Snapshot( static_cast< DWORD >( T ), id );

            if ( hSnapshot == INVALID_HANDLE_VALUE || !hSnapshot )
                throw error::from_win32( GetLastError() );

            return handle_t::create( hSnapshot );
        }

        /// <summary>
        /// Creates a new snapshot from a snapshot handle.
        /// </summary>
        /// <param name="handle">The handle of the snapshot.</param>
        snapshot( std::shared_ptr< handle_t > handle ) : handle( handle )
        {
        }

        /// <summary>
        /// Returns the iterator for the beginning of the snapshot.
        /// </summary>
        /// <returns>Iterator.</returns>
        iterator begin() const
        {
            return iterator( handle );
        }

        /// <summary>
        /// Returns the end of the iterator.
        /// </summary>
        /// <returns>Iterator.</returns>
        iterator end() const
        {
            return iterator( nullptr );
        }
    };

    /// <summary>
    /// Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
    /// </summary>
    struct process_entry_t
    {
        /// <summary>
        /// Default constructor.
        /// </summary>
        process_entry_t() = default;

        /// <summary>
        /// The process identifier.
        /// </summary>
        std::uint32_t id;

        /// <summary>
        /// The number of execution threads started by the process.
        /// </summary>
        std::uint32_t threads;

        /// <summary>
        /// The process identifier of the parent process.
        /// </summary>
        std::uint32_t parent_id;

        /// <summary>
        /// The base priority of any threads created by this process.
        /// </summary>
        std::int32_t priority;

        /// <summary>
        /// The name of the process.
        /// </summary>
        std::string name;
    };

    /// <summary>
    /// Specialization for the process snapshot.
    /// </summary>
    template<>
    class snapshot< snapshot_kind::process_t >::iterator
    {
        std::shared_ptr< handle_t > handle;
        PROCESSENTRY32 entry;
        mutable process_entry_t result;

       public:
        /// <summary>
        /// Creates a new iterator for the snapshot class.
        /// </summary>
        /// <param name="handle">The handle to the snapshot.</param>
        explicit iterator( std::shared_ptr< handle_t > handle );

        /// <summary>
        /// Implements the `*` operator for the iterator.
        /// </summary>
        /// <returns>Reference to the process entry.</returns>
        process_entry_t& operator*() const noexcept;

        /// <summary>
        /// Implements the `->` operator for the iterator.
        /// </summary>
        /// <returns>Pointer to the process entry.</returns>
        process_entry_t* operator->() const noexcept;

        /// <summary>
        /// Implements the `++` operator for the iterator.
        /// </summary>
        /// <returns>The new iterator with the next entry.</returns>
        iterator& operator++();

        /// <summary>
        /// Compares the current iterator with another.
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>True if the same.</returns>
        bool operator==( const iterator& other ) const noexcept;

        /// <summary>
        /// Compares the current iterator with another (not equals).
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>True if different.</returns>
        bool operator!=( const iterator& other ) const noexcept;
    };

    /// <summary>
    /// Describes an entry from a list of the threads residing in the system address space when a snapshot was taken.
    /// </summary>
    struct thread_entry_t
    {
        /// <summary>
        /// Default constructor.
        /// </summary>
        thread_entry_t() = default;

        /// <summary>
        /// The thread identifier, compatible with the thread identifier returned by the CreateProcess function.
        /// </summary>
        std::uint32_t id;

        /// <summary>
        /// The identifier of the process that created the thread.
        /// </summary>
        std::uint32_t owner_id;

        /// <summary>
        /// The kernel base priority level assigned to the thread. The priority is a number from 0 to 31, with 0 representing the lowest possible
        /// thread priority. For more information, see KeQueryPriorityThread.
        /// </summary>
        std::uint32_t base_priority;
    };

    /// <summary>
    /// Specialization for the thread snapshot.
    /// </summary>
    template<>
    class snapshot< snapshot_kind::thread_t >::iterator
    {
        std::shared_ptr< handle_t > handle;
        THREADENTRY32 entry;
        mutable thread_entry_t result;

       public:
        /// <summary>
        /// Creates a new iterator for the snapshot class.
        /// </summary>
        /// <param name="handle">The handle to the snapshot.</param>
        explicit iterator( std::shared_ptr< handle_t > handle );

        /// <summary>
        /// Implements the `*` operator for the iterator.
        /// </summary>
        /// <returns>Reference to the process entry.</returns>
        thread_entry_t& operator*() const noexcept;

        /// <summary>
        /// Implements the `->` operator for the iterator.
        /// </summary>
        /// <returns>Pointer to the process entry.</returns>
        thread_entry_t* operator->() const noexcept;

        /// <summary>
        /// Implements the `++` operator for the iterator.
        /// </summary>
        /// <returns>The new iterator with the next entry.</returns>
        iterator& operator++();

        /// <summary>
        /// Compares the current iterator with another.
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>True if the same.</returns>
        bool operator==( const iterator& other ) const noexcept;

        /// <summary>
        /// Compares the current iterator with another (not equals).
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>True if different.</returns>
        bool operator!=( const iterator& other ) const noexcept;
    };

    /// <summary>
    /// Describes an entry from a list of the modules belonging to the specified process.
    /// </summary>
    struct module_entry_t
    {
        /// <summary>
        /// Creates a new module entry.
        /// </summary>
        module_entry_t( const MODULEENTRY32& entry );

        /// <summary>
        /// Creates a copy of the module entry.
        /// </summary>
        module_entry_t( const module_entry_t& ) = default;

        /// <summary>
        /// The identifier of the process whose modules are to be examined.
        /// </summary>
        std::uint32_t process_id;

        /// <summary>
        /// The load count of the module, which is not generally meaningful, and usually equal to 0xFFFF.
        /// </summary>
        std::uint32_t usage_count;

        /// <summary>
        /// The base address of the module in the context of the owning process.
        /// </summary>
        std::uintptr_t base_address;

        /// <summary>
        /// The size of the module, in bytes.
        /// </summary>
        std::uint32_t base_size;

        /// <summary>
        /// Tha name of the module.
        /// </summary>
        std::string name;

        /// <summary>
        /// The path of the module.
        /// </summary>
        std::string path;
    };

    /// <summary>
    /// Specialization for the module snapshot.
    /// </summary>
    template<>
    class snapshot< snapshot_kind::module_t >::iterator
    {
        std::shared_ptr< handle_t > handle;
        MODULEENTRY32 entry;

       public:
        /// <summary>
        /// Creates a new iterator for the snapshot class.
        /// </summary>
        /// <param name="handle">The handle to the snapshot.</param>
        explicit iterator( std::shared_ptr< handle_t > handle );

        /// <summary>
        /// Implements the `*` operator for the iterator.
        /// </summary>
        /// <returns>Reference to the process entry.</returns>
        module_entry_t operator*() const noexcept;

        /// <summary>
        /// Implements the `++` operator for the iterator.
        /// </summary>
        /// <returns>The new iterator with the next entry.</returns>
        iterator& operator++();

        /// <summary>
        /// Compares the current iterator with another.
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>True if the same.</returns>
        bool operator==( const iterator& other ) const noexcept;

        /// <summary>
        /// Compares the current iterator with another (not equals).
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>True if different.</returns>
        bool operator!=( const iterator& other ) const noexcept;
    };
}  // namespace wincpp::core