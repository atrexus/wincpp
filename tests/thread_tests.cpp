#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <wincpp/wincpp.hpp>

TEST_CASE( "Thread factory enumerates threads owned by the process", "[threads]" )
{
    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto threads = process->thread_factory.threads().vector();

    REQUIRE_FALSE( threads.empty() );
    CHECK( std::ranges::all_of( threads, [ & ]( const auto& thread ) { return thread.process_id() == process->id(); } ) );

    const auto& first = threads.front();
    const auto fetched = process->thread_factory.fetch_thread( first.id() );

    REQUIRE( fetched.has_value() );
    CHECK( fetched->id() == first.id() );
    CHECK( fetched->process_id() == process->id() );

    const auto by_index = process->thread_factory[ first.id() ];

    CHECK( by_index.id() == first.id() );

    const auto handle = first.handle();
    REQUIRE( handle != nullptr );
    CHECK( handle->native != nullptr );
}

TEST_CASE( "Thread factory can scope a suspend operation", "[threads]" )
{
    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    auto guard = process->thread_factory.scoped_suspend_all();

    CHECK( guard.active() );
    CHECK( guard.size() <= process->thread_factory.threads().vector().size() );

    guard.resume();

    CHECK_FALSE( guard.active() );
}
