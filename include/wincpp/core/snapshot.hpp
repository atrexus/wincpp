#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace wincpp::core
{
    /// <summary>
    /// The kind of snapshot we can take.
    /// </summary>
    enum class snapshot_kind : std::uint32_t
    {
        /// <summary>
        /// Includes all heaps of the process specified in the snapshot.
        /// </summary>
        heaplist_t = 0x00000001,

        /// <summary>
        /// Includes all processes in the system in the snapshot.
        /// </summary>
        process_t = 0x00000002,

        /// <summary>
        /// Includes all threads in the system in the snapshot.
        /// </summary>
        thread_t = 0x00000004,

        /// <summary>
        /// Includes all modules of the process specified in the snapshot.
        /// </summary>
        module_t = 0x00000008,

        /// <summary>
        /// Includes all 32-bit modules of the process specified in the snapshot when called from a 64-bit process.
        /// </summary>
        module32_t = 0x00000010
    };

    /// <summary>
    /// Describes an entry from a list of the processes residing in the system address space when a snapshot was taken.
    /// </summary>
    struct process_entry_t
    {
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
    /// Describes an entry from a list of the threads residing in the system address space when a snapshot was taken.
    /// </summary>
    struct thread_entry_t
    {
        /// <summary>
        /// The thread identifier.
        /// </summary>
        std::uint32_t id;

        /// <summary>
        /// The identifier of the process that created the thread.
        /// </summary>
        std::uint32_t owner_id;

        /// <summary>
        /// The kernel base priority level assigned to the thread.
        /// </summary>
        std::uint32_t base_priority;
    };

    /// <summary>
    /// Describes an entry from a list of the modules belonging to the specified process.
    /// </summary>
    struct module_entry_t
    {
        /// <summary>
        /// The identifier of the process whose modules are to be examined.
        /// </summary>
        std::uint32_t process_id;

        /// <summary>
        /// The load count of the module.
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
        /// The name of the module.
        /// </summary>
        std::string name;

        /// <summary>
        /// The path of the module.
        /// </summary>
        std::string path;
    };

    /// <summary>
    /// Represents a materialized snapshot created by the tool help library.
    /// </summary>
    template< snapshot_kind T >
    class snapshot;

    /// <summary>
    /// Represents a materialized process snapshot.
    /// </summary>
    template<>
    class snapshot< snapshot_kind::process_t > final
    {
       public:
        /// <summary>
        /// The entry type in the snapshot.
        /// </summary>
        using value_type = process_entry_t;

        /// <summary>
        /// The snapshot iterator type.
        /// </summary>
        using iterator = std::vector< value_type >::const_iterator;

        /// <summary>
        /// Creates a new process snapshot.
        /// </summary>
        /// <param name="id">The optional process identifier.</param>
        /// <returns>The snapshot.</returns>
        static snapshot create( std::uint32_t id = 0 );

        /// <summary>
        /// Creates a new process snapshot from entries.
        /// </summary>
        /// <param name="entries">The snapshot entries.</param>
        explicit snapshot( std::vector< value_type > entries );

        /// <summary>
        /// Returns the iterator for the beginning of the snapshot.
        /// </summary>
        /// <returns>The begin iterator.</returns>
        iterator begin() const noexcept;

        /// <summary>
        /// Returns the end of the iterator.
        /// </summary>
        /// <returns>The end iterator.</returns>
        iterator end() const noexcept;

       private:
        std::vector< value_type > entries;
    };

    /// <summary>
    /// Represents a materialized thread snapshot.
    /// </summary>
    template<>
    class snapshot< snapshot_kind::thread_t > final
    {
       public:
        /// <summary>
        /// The entry type in the snapshot.
        /// </summary>
        using value_type = thread_entry_t;

        /// <summary>
        /// The snapshot iterator type.
        /// </summary>
        using iterator = std::vector< value_type >::const_iterator;

        /// <summary>
        /// Creates a new thread snapshot.
        /// </summary>
        /// <param name="id">The optional process identifier.</param>
        /// <returns>The snapshot.</returns>
        static snapshot create( std::uint32_t id = 0 );

        /// <summary>
        /// Creates a new thread snapshot from entries.
        /// </summary>
        /// <param name="entries">The snapshot entries.</param>
        explicit snapshot( std::vector< value_type > entries );

        /// <summary>
        /// Returns the iterator for the beginning of the snapshot.
        /// </summary>
        /// <returns>The begin iterator.</returns>
        iterator begin() const noexcept;

        /// <summary>
        /// Returns the end of the iterator.
        /// </summary>
        /// <returns>The end iterator.</returns>
        iterator end() const noexcept;

       private:
        std::vector< value_type > entries;
    };

    /// <summary>
    /// Represents a materialized module snapshot.
    /// </summary>
    template<>
    class snapshot< snapshot_kind::module_t > final
    {
       public:
        /// <summary>
        /// The entry type in the snapshot.
        /// </summary>
        using value_type = module_entry_t;

        /// <summary>
        /// The snapshot iterator type.
        /// </summary>
        using iterator = std::vector< value_type >::const_iterator;

        /// <summary>
        /// Creates a new module snapshot.
        /// </summary>
        /// <param name="id">The optional process identifier.</param>
        /// <returns>The snapshot.</returns>
        static snapshot create( std::uint32_t id = 0 );

        /// <summary>
        /// Creates a new module snapshot from entries.
        /// </summary>
        /// <param name="entries">The snapshot entries.</param>
        explicit snapshot( std::vector< value_type > entries );

        /// <summary>
        /// Returns the iterator for the beginning of the snapshot.
        /// </summary>
        /// <returns>The begin iterator.</returns>
        iterator begin() const noexcept;

        /// <summary>
        /// Returns the end of the iterator.
        /// </summary>
        /// <returns>The end iterator.</returns>
        iterator end() const noexcept;

       private:
        std::vector< value_type > entries;
    };
}  // namespace wincpp::core

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/core/snapshot.inl"
#endif
