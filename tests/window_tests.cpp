#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <wincpp/wincpp.hpp>

TEST_CASE( "Window factory safely handles processes with or without visible windows", "[windows]" )
{
    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto windows = process->window_factory.windows();

    CHECK(
        std::ranges::all_of( windows, [ & ]( const auto& window ) { return window.handle() != nullptr && window.process_id() == process->id(); } ) );

    const auto main_window = process->window_factory.main_window();

    if ( !main_window )
    {
        SUCCEED( "The current test process does not own a visible top-level window." );
        return;
    }

    CHECK( main_window->process_id() == process->id() );
    CHECK_NOTHROW( main_window->title() );
    CHECK_NOTHROW( main_window->class_name() );
    CHECK_NOTHROW( main_window->is_active() );
    CHECK_NOTHROW( main_window->placement() );

    const auto thread = main_window->thread();

    CHECK( thread.process_id() == process->id() );
}
