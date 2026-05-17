#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace wincpp::threads
{
    struct thread_t;
    class thread_list;
}  // namespace wincpp::threads

namespace wincpp
{
    struct process_t;
    class thread_factory;

    /// <summary>
    /// Scoped guard that resumes a set of suspended process threads when it is destroyed.
    /// </summary>
    class thread_suspend_guard final
    {
        friend class thread_factory;

       public:
        /// <summary>
        /// Destroys the guard and resumes any threads still owned by the guard.
        /// </summary>
        ~thread_suspend_guard();

        /// <summary>
        /// Creates an empty guard.
        /// </summary>
        thread_suspend_guard() noexcept;

        /// <summary>
        /// Moves a thread suspend guard.
        /// </summary>
        /// <param name="other">The guard to move from.</param>
        thread_suspend_guard( thread_suspend_guard&& other ) noexcept;

        /// <summary>
        /// Moves a thread suspend guard.
        /// </summary>
        /// <param name="other">The guard to move from.</param>
        /// <returns>This guard.</returns>
        thread_suspend_guard& operator=( thread_suspend_guard&& other ) noexcept;

        thread_suspend_guard( const thread_suspend_guard& ) = delete;
        thread_suspend_guard& operator=( const thread_suspend_guard& ) = delete;

        /// <summary>
        /// Resumes all threads owned by the guard and deactivates it.
        /// </summary>
        void resume() noexcept;

        /// <summary>
        /// Releases ownership of the suspended threads without resuming them.
        /// </summary>
        void release() noexcept;

        /// <summary>
        /// Gets the number of threads suspended by this guard.
        /// </summary>
        /// <returns>The number of suspended threads owned by the guard.</returns>
        std::size_t size() const noexcept;

        /// <summary>
        /// Determines whether the guard will resume threads on destruction.
        /// </summary>
        /// <returns>True if the guard is active.</returns>
        bool active() const noexcept;

       private:
        /// <summary>
        /// Creates a guard for a thread factory.
        /// </summary>
        /// <param name="factory">The thread factory that owns the suspended threads.</param>
        explicit thread_suspend_guard( const thread_factory& factory ) noexcept;

        const thread_factory* factory;
        std::vector< std::uint32_t > thread_ids;
        bool is_active;
    };

    /// <summary>
    /// The factory that creates and manages threads.
    /// </summary>
    class thread_factory final
    {
        friend struct process_t;

        process_t* p;

        /// <summary>
        /// Creates a new thread factory object.
        /// </summary>
        /// <param name="process">The process object.</param>
        explicit thread_factory( process_t* p ) noexcept;

       public:
        /// <summary>
        /// Gets a list of threads in the process.
        /// </summary>
        /// <returns>The list of threads.</returns>
        threads::thread_list threads() const;

        /// <summary>
        /// Suspends all threads in the process.
        /// </summary>
        void suspend_all() const;

        /// <summary>
        /// Suspends all threads in the process and returns a guard that resumes them automatically.
        /// </summary>
        /// <param name="include_current_thread">Whether the current thread should also be suspended.</param>
        /// <returns>A guard that owns the suspended threads.</returns>
        thread_suspend_guard scoped_suspend_all( bool include_current_thread = false ) const;

        /// <summary>
        /// Resumes all threads in the process.
        /// </summary>
        void resume_all() const;

        /// <summary>
        /// Gets the thread by its id.
        /// </summary>
        /// <param name="index">The id of the thread.</param>
        /// <returns>The thread.</returns>
        std::optional< threads::thread_t > fetch_thread( std::size_t index ) const;

        /// <summary>
        /// Gets the thread by its id.
        /// </summary>
        /// <param name="index">The id of the thread.</param>
        /// <returns>The thread.</returns>
        threads::thread_t operator[]( std::size_t index ) const;
    };
}  // namespace wincpp

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/thread_factory.inl"
#endif
