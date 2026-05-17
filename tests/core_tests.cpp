#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <wincpp/wincpp.hpp>

TEST_CASE( "Core geometry helpers preserve Win32-style dimensions", "[core]" )
{
    auto rectangle = wincpp::core::rectangle_t{ 10, 20, 35, 65 };

    CHECK( rectangle.width() == 25 );
    CHECK( rectangle.height() == 45 );

    rectangle.set_width( 100 );
    rectangle.set_height( 200 );

    CHECK( rectangle.left == 10 );
    CHECK( rectangle.top == 20 );
    CHECK( rectangle.right == 110 );
    CHECK( rectangle.bottom == 220 );

    const auto point = wincpp::core::point_t{ 4, 9 };

    CHECK( point.x == 4 );
    CHECK( point.y == 9 );
}

TEST_CASE( "Core handles and errors wrap native state without leaking SDK types", "[core]" )
{
    const auto handle = wincpp::core::handle_t::create( nullptr, false );

    REQUIRE( handle != nullptr );
    CHECK( handle->native == nullptr );

    const auto user_error = wincpp::core::error::from_user( wincpp::core::user_error_type_t::process_not_found_t, "Failed to find process {}", 42 );

    CHECK( std::string{ user_error.code().category().name() } == "user" );
    CHECK( user_error.code().message() == "The desired process was not found." );
    CHECK( std::string{ user_error.what() }.find( "Failed to find process 42" ) != std::string::npos );

    const auto win32_error = wincpp::core::error::from_win32( 2 );

    CHECK( std::string{ win32_error.code().category().name() } == "win32" );
    CHECK_FALSE( win32_error.code().message().empty() );
}

TEST_CASE( "Process access flags combine without exposing Windows macros", "[core]" )
{
    auto access = wincpp::core::process_access_t::query_information_t | wincpp::core::process_access_t::vm_read_t;

    CHECK( wincpp::core::has_flag( access, wincpp::core::process_access_t::query_information_t ) );
    CHECK( wincpp::core::has_flag( access, wincpp::core::process_access_t::vm_read_t ) );
    CHECK_FALSE( wincpp::core::has_flag( access, wincpp::core::process_access_t::vm_write_t ) );

    access |= wincpp::core::process_access_t::synchronize_t;

    CHECK( wincpp::core::has_flag( access, wincpp::core::process_access_t::synchronize_t ) );
    CHECK( ( access & wincpp::core::process_access_t::vm_read_t ) == wincpp::core::process_access_t::vm_read_t );
}

TEST_CASE( "Toolhelp snapshots expose the current process, modules, and threads", "[core][snapshot]" )
{
    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto processes = wincpp::core::snapshot< wincpp::core::snapshot_kind::process_t >::create();
    CHECK( std::ranges::any_of( processes, [ & ]( const auto& entry ) { return entry.id == process->id(); } ) );

    const auto threads = wincpp::core::snapshot< wincpp::core::snapshot_kind::thread_t >::create();
    CHECK( std::ranges::any_of( threads, [ & ]( const auto& entry ) { return entry.owner_id == process->id(); } ) );

    const auto modules = wincpp::core::snapshot< wincpp::core::snapshot_kind::module_t >::create( process->id() );
    CHECK( std::ranges::any_of( modules, [ & ]( const auto& entry ) { return entry.process_id == process->id(); } ) );
}
