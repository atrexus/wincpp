#include <catch2/catch_test_macros.hpp>
#include <string>
#include <wincpp/wincpp.hpp>

TEST_CASE( "Process factory creates a lightweight wrapper for the current process", "[process]" )
{
    const auto current = wincpp::process_t::current();

    REQUIRE( current != nullptr );
    CHECK( current->id() != 0 );
    CHECK_FALSE( current->name().empty() );
    REQUIRE( current->handle != nullptr );
    CHECK( current->handle->native != nullptr );
}

TEST_CASE( "Process factory can reopen the current process by id", "[process]" )
{
    const auto current = wincpp::process_t::current();
    REQUIRE( current != nullptr );

    const auto opened = wincpp::process_t::open( current->id() );

    REQUIRE( opened != nullptr );
    CHECK( opened->id() == current->id() );
    CHECK( std::string{ opened->name() } == std::string{ current->name() } );
    REQUIRE( opened->handle != nullptr );
    CHECK( opened->handle->native != nullptr );
}

TEST_CASE( "Process factory exposes non-throwing open helpers", "[process]" )
{
    const auto current = wincpp::process_t::try_current();

    REQUIRE( current.has_value() );
    REQUIRE( *current != nullptr );

    const auto access = wincpp::core::process_access_t::query_limited_information_t | wincpp::core::process_access_t::synchronize_t;
    const auto opened = wincpp::process_t::try_open( ( *current )->id(), access );

    REQUIRE( opened.has_value() );
    REQUIRE( *opened != nullptr );
    CHECK( ( *opened )->id() == ( *current )->id() );

    const auto missing = wincpp::process_t::try_open( 0xFFFFFFFF );

    REQUIRE_FALSE( missing.has_value() );
    CHECK( missing.error().code().message() == "The desired process was not found." );
}

TEST_CASE( "Process factory reports missing processes through wincpp errors", "[process]" )
{
    try
    {
        [[maybe_unused]] const auto missing_process = wincpp::process_t::open( 0xFFFFFFFF );
        FAIL( "Expected process_t::open to throw for a missing process id." );
    }
    catch ( const wincpp::core::error& error )
    {
        CHECK( std::string{ error.code().category().name() } == "user" );
        CHECK( error.code().message() == "The desired process was not found." );
    }
}
