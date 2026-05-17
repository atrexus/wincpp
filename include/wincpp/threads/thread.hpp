#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "wincpp/core/snapshot.hpp"
#include "wincpp/core/win.hpp"

namespace wincpp
{
    class thread_factory;
    struct process_t;
}  // namespace wincpp

namespace wincpp::threads
{
    /// <summary>
    /// Opaque thread context storage.
    /// </summary>
    struct thread_context_t
    {
        /// <summary>
        /// The requested context flags. A value of 0 means the default full context was used.
        /// </summary>
        std::uint64_t flags;

        /// <summary>
        /// The native context bytes.
        /// </summary>
        std::vector< std::byte > bytes;
    };

    /// <summary>
    /// Represents a thread object. Can be remote or local.
    /// </summary>
    struct thread_t
    {
        friend class thread_list;
        friend class wincpp::thread_factory;

        /// <summary>
        /// Gets the handle to the thread.
        /// </summary>
        /// <returns>The handle to the thread.</returns>
        const std::shared_ptr< core::handle_t > handle() const;

        /// <summary>
        /// Gets the id of the thread.
        /// </summary>
        /// <returns>The id of the thread.</returns>
        constexpr std::uint32_t id() const noexcept;

        /// <summary>
        /// Gets the process id of the thread.
        /// </summary>
        /// <returns>The process id of the thread.</returns>
        constexpr std::uint32_t process_id() const noexcept;

        /// <summary>
        /// Suspends the thread.
        /// </summary>
        /// <returns>The previous suspend count.</returns>
        std::size_t suspend() const;

        /// <summary>
        /// Resumes the thread.
        /// </summary>
        /// <returns>The previous suspend count.</returns>
        std::size_t resume() const;

        /// <summary>
        /// Gets the thread context.
        /// </summary>
        /// <param name="flags">The context flags. A value of 0 requests the default full context.</param>
        /// <returns>The thread context.</returns>
        thread_context_t context( std::uint64_t flags = 0 ) const;

       protected:
        /// <summary>
        /// Creates a new thread object.
        /// </summary>
        /// <param name="entry">The thread entry.</param>
        explicit thread_t( const core::thread_entry_t& entry );

        core::thread_entry_t entry;
        mutable std::shared_ptr< core::handle_t > hthread;
    };

    /// <summary>
    /// Represents a list of threads in the remote process.
    /// </summary>
    class thread_list final
    {
        friend class wincpp::thread_factory;

        core::snapshot< core::snapshot_kind::thread_t > snapshot;
        process_t* process;

        /// <summary>
        /// Creates a new thread list object.
        /// </summary>
        /// <param name="process">The process object.</param>
        explicit thread_list( process_t* process );

       public:
        /// <summary>
        /// The iterator for the thread list.
        /// </summary>
        class iterator;

        /// <summary>
        /// Gets the begin iterator for the thread list.
        /// </summary>
        /// <returns>The begin iterator.</returns>
        iterator begin() const;

        /// <summary>
        /// Gets the end iterator for the thread list.
        /// </summary>
        /// <returns>The end iterator.</returns>
        iterator end() const noexcept;

        /// <summary>
        /// Gets the thread list as a vector.
        /// </summary>
        /// <returns>The thread vector.</returns>
        std::vector< thread_t > vector() const;
    };

    /// <summary>
    /// Represents a thread list iterator.
    /// </summary>
    class thread_list::iterator final
    {
        friend class thread_list;

        core::snapshot< core::snapshot_kind::thread_t >::iterator it;
        core::snapshot< core::snapshot_kind::thread_t >::iterator last;
        process_t* process;

        /// <summary>
        /// Creates a new thread iterator object.
        /// </summary>
        /// <param name="process">The process object.</param>
        /// <param name="it">The thread iterator.</param>
        /// <param name="last">The end iterator.</param>
        iterator(
            process_t* process,
            const core::snapshot< core::snapshot_kind::thread_t >::iterator& it,
            const core::snapshot< core::snapshot_kind::thread_t >::iterator& last ) noexcept;

       public:
        /// <summary>
        /// Gets the thread object.
        /// </summary>
        /// <returns>The thread object.</returns>
        thread_t operator*() const noexcept;

        /// <summary>
        /// Moves to the next thread.
        /// </summary>
        /// <returns>The next thread iterator.</returns>
        iterator& operator++();

        /// <summary>
        /// Compares two iterators.
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>Whether the iterators are equal.</returns>
        bool operator==( const iterator& other ) const noexcept;

        /// <summary>
        /// Compares two iterators.
        /// </summary>
        /// <param name="other">The other iterator.</param>
        /// <returns>Whether the iterators are not equal.</returns>
        bool operator!=( const iterator& other ) const noexcept;

       private:
        void skip_foreign_threads();
    };
}  // namespace wincpp::threads

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/threads/thread.inl"
#endif
