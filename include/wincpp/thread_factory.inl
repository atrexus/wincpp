#pragma once

#include <utility>

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/process.hpp"
#include "wincpp/thread_factory.hpp"
#include "wincpp/threads/thread.hpp"

namespace wincpp
{
    inline thread_suspend_guard::~thread_suspend_guard()
    {
        resume();
    }

    inline thread_suspend_guard::thread_suspend_guard() noexcept : factory( nullptr ), is_active( false )
    {
    }

    inline thread_suspend_guard::thread_suspend_guard( const thread_factory& factory ) noexcept : factory( &factory ), is_active( true )
    {
    }

    inline thread_suspend_guard::thread_suspend_guard( thread_suspend_guard&& other ) noexcept
        : factory( other.factory ),
          thread_ids( std::move( other.thread_ids ) ),
          is_active( other.is_active )
    {
        other.factory = nullptr;
        other.is_active = false;
    }

    inline thread_suspend_guard& thread_suspend_guard::operator=( thread_suspend_guard&& other ) noexcept
    {
        if ( this != &other )
        {
            resume();

            factory = other.factory;
            thread_ids = std::move( other.thread_ids );
            is_active = other.is_active;

            other.factory = nullptr;
            other.is_active = false;
        }

        return *this;
    }

    inline void thread_suspend_guard::resume() noexcept
    {
        if ( !is_active || !factory )
            return;

        for ( auto it = thread_ids.rbegin(); it != thread_ids.rend(); ++it )
        {
            try
            {
                if ( const auto thread = factory->fetch_thread( *it ) )
                    thread->resume();
            }
            catch ( ... )
            {
            }
        }

        is_active = false;
    }

    inline void thread_suspend_guard::release() noexcept
    {
        is_active = false;
        thread_ids.clear();
    }

    inline std::size_t thread_suspend_guard::size() const noexcept
    {
        return thread_ids.size();
    }

    inline bool thread_suspend_guard::active() const noexcept
    {
        return is_active;
    }

    inline thread_factory::thread_factory( process_t* const p ) noexcept : p( p )
    {
    }

    inline threads::thread_list thread_factory::threads() const
    {
        return threads::thread_list( p );
    }

    inline void thread_factory::suspend_all() const
    {
        for ( const auto& thread : threads() )
        {
            thread.suspend();
        }
    }

    inline thread_suspend_guard thread_factory::scoped_suspend_all( const bool include_current_thread ) const
    {
        thread_suspend_guard guard{ *this };
        const auto current_thread_id = GetCurrentThreadId();

        try
        {
            for ( const auto& thread : threads() )
            {
                if ( !include_current_thread && thread.id() == current_thread_id )
                    continue;

                thread.suspend();
                guard.thread_ids.push_back( thread.id() );
            }
        }
        catch ( ... )
        {
            guard.resume();
            throw;
        }

        return guard;
    }

    inline void thread_factory::resume_all() const
    {
        for ( const auto& thread : threads() )
        {
            thread.resume();
        }
    }

    inline std::optional< threads::thread_t > thread_factory::fetch_thread( const std::size_t index ) const
    {
        for ( const auto& thread : threads() )
        {
            if ( thread.id() == index )
                return thread;
        }

        return std::nullopt;
    }

    inline threads::thread_t thread_factory::operator[]( const std::size_t index ) const
    {
        if ( const auto result = fetch_thread( index ) )
            return *result;

        throw core::error::from_user( core::user_error_type_t::thread_not_found_t, "Failed to find thread with id {}", index );
    }
}  // namespace wincpp
