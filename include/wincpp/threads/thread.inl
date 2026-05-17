#pragma once

#include <cstring>

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/process.hpp"
#include "wincpp/threads/thread.hpp"

namespace wincpp::threads
{
    constexpr std::uint32_t thread_t::id() const noexcept
    {
        return entry.id;
    }

    constexpr std::uint32_t thread_t::process_id() const noexcept
    {
        return entry.owner_id;
    }

    inline std::size_t thread_t::suspend() const
    {
        const auto result = SuspendThread( static_cast< HANDLE >( handle()->native ) );

        if ( result == static_cast< DWORD >( -1 ) )
            throw core::error::from_win32( GetLastError() );

        return result;
    }

    inline std::size_t thread_t::resume() const
    {
        const auto result = ResumeThread( static_cast< HANDLE >( handle()->native ) );

        if ( result == static_cast< DWORD >( -1 ) )
            throw core::error::from_win32( GetLastError() );

        return result;
    }

    inline thread_context_t thread_t::context( const std::uint64_t flags ) const
    {
        CONTEXT context{};
        context.ContextFlags = flags == 0 ? CONTEXT_ALL : static_cast< DWORD >( flags );

        if ( !GetThreadContext( static_cast< HANDLE >( handle()->native ), &context ) )
            throw core::error::from_win32( GetLastError() );

        thread_context_t result{ .flags = flags, .bytes = std::vector< std::byte >( sizeof( CONTEXT ) ) };
        std::memcpy( result.bytes.data(), &context, sizeof( CONTEXT ) );
        return result;
    }

    inline thread_t::thread_t( const core::thread_entry_t& entry ) : entry( entry )
    {
    }

    inline const std::shared_ptr< core::handle_t > thread_t::handle() const
    {
        if ( hthread == nullptr )
        {
            const auto thread = OpenThread( THREAD_ALL_ACCESS, FALSE, entry.id );

            if ( !thread )
                throw core::error::from_win32( GetLastError() );

            hthread = core::handle_t::create( thread );
        }

        return hthread;
    }

    inline thread_list::thread_list( process_t* const process )
        : snapshot( core::snapshot< core::snapshot_kind::thread_t >::create() ),
          process( process )
    {
    }

    inline thread_list::iterator thread_list::begin() const
    {
        auto iterator = thread_list::iterator( process, snapshot.begin(), snapshot.end() );
        iterator.skip_foreign_threads();
        return iterator;
    }

    inline thread_list::iterator thread_list::end() const noexcept
    {
        return { process, snapshot.end(), snapshot.end() };
    }

    inline std::vector< thread_t > thread_list::vector() const
    {
        std::vector< thread_t > threads;

        for ( auto it = begin(); it != end(); ++it )
        {
            threads.push_back( *it );
        }

        return threads;
    }

    inline thread_list::iterator::iterator(
        process_t* const process,
        const core::snapshot< core::snapshot_kind::thread_t >::iterator& it,
        const core::snapshot< core::snapshot_kind::thread_t >::iterator& last ) noexcept
        : it( it ),
          last( last ),
          process( process )
    {
    }

    inline thread_t thread_list::iterator::operator*() const noexcept
    {
        return thread_t{ *it };
    }

    inline thread_list::iterator& thread_list::iterator::operator++()
    {
        if ( it != last )
        {
            ++it;
            skip_foreign_threads();
        }

        return *this;
    }

    inline bool thread_list::iterator::operator==( const iterator& other ) const noexcept
    {
        return it == other.it;
    }

    inline bool thread_list::iterator::operator!=( const iterator& other ) const noexcept
    {
        return !( *this == other );
    }

    inline void thread_list::iterator::skip_foreign_threads()
    {
        while ( it != last && it->owner_id != process->id() )
        {
            ++it;
        }
    }
}  // namespace wincpp::threads
