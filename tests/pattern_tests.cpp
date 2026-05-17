#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <span>
#include <sstream>
#include <string>
#include <vector>
#include <wincpp/wincpp.hpp>

namespace
{
    template< wincpp::patterns::scanner::algorithm_t Algorithm >
    void check_exact_pattern_algorithm()
    {
        const auto bytes = std::array< std::uint8_t, 9 >{ 0x90, 0x48, 0x8D, 0x11, 0x90, 0x90, 0x48, 0x8D, 0x11 };
        const auto pattern = wincpp::patterns::pattern_t{ "\x48\x8D\x11", "xxx" };

        const auto first = wincpp::patterns::scanner::find< Algorithm >( std::span< const std::uint8_t >{ bytes }, pattern );
        const auto all = wincpp::patterns::scanner::find_all< Algorithm >( std::span< const std::uint8_t >{ bytes }, pattern );

        REQUIRE( first.has_value() );
        CHECK( *first == 1 );
        CHECK( all == std::vector< std::uintptr_t >{ 1, 6 } );
    }
}  // namespace

TEST_CASE( "Patterns preserve byte masks and string formatting", "[patterns]" )
{
    const auto ida_pattern = wincpp::patterns::pattern_t{ "\x48\x8D\x00", "xx?" };

    CHECK( ida_pattern.size == 3 );
    CHECK( ida_pattern.to_string() == "488D??" );

    std::stringstream stream;
    stream << ida_pattern;

    CHECK( stream.str() == ida_pattern.to_string() );

    const auto string_pattern = wincpp::patterns::pattern_t{ std::string{ "AB" } };
    CHECK( string_pattern.to_string() == "4142" );
}

TEST_CASE( "Patterns parse IDA-style strings", "[patterns]" )
{
    const auto pattern = wincpp::patterns::pattern_t::from_ida( "48 8D 0D ? ? 90" );

    CHECK( pattern.size == 6 );
    CHECK( pattern.to_string() == "488D0D????90" );

    const auto parsed = wincpp::patterns::pattern_t::try_from_ida( "AA BB ?? CC" );

    REQUIRE( parsed.has_value() );
    CHECK( parsed->to_string() == "AABB??CC" );

    CHECK_FALSE( wincpp::patterns::pattern_t::try_from_ida( "AA GG" ).has_value() );
    CHECK_THROWS_AS( wincpp::patterns::pattern_t::from_ida( "AA GG" ), wincpp::core::error );
}

TEST_CASE( "Scanner algorithms find exact matches consistently", "[patterns][scanner]" )
{
    check_exact_pattern_algorithm< wincpp::patterns::scanner::algorithm_t::naive_t >();
    check_exact_pattern_algorithm< wincpp::patterns::scanner::algorithm_t::bmh_t >();
    check_exact_pattern_algorithm< wincpp::patterns::scanner::algorithm_t::raita_t >();
    check_exact_pattern_algorithm< wincpp::patterns::scanner::algorithm_t::tbm_t >();
}

TEST_CASE( "Scanner dispatches algorithms at runtime", "[patterns][scanner]" )
{
    const auto bytes = std::array< std::uint8_t, 7 >{ 0x90, 0xAA, 0xBB, 0xCC, 0x90, 0xAA, 0xBB };
    const auto pattern = wincpp::patterns::pattern_t::from_ida( "AA BB" );

    const auto first =
        wincpp::patterns::scanner::find( std::span< const std::uint8_t >{ bytes }, pattern, wincpp::patterns::scanner::algorithm_t::raita_t );
    const auto all =
        wincpp::patterns::scanner::find_all( std::span< const std::uint8_t >{ bytes }, pattern, wincpp::patterns::scanner::algorithm_t::tbm_t );

    REQUIRE( first.has_value() );
    CHECK( *first == 1 );
    CHECK( all == std::vector< std::uintptr_t >{ 1, 5 } );
}

TEST_CASE( "Scanner supports wildcard masks", "[patterns][scanner]" )
{
    const auto bytes = std::array< std::uint8_t, 9 >{ 0x90, 0x48, 0x8D, 0x01, 0x02, 0x03, 0x48, 0x8D, 0x04 };
    const auto pattern = wincpp::patterns::pattern_t{ "\x48\x8D\x00", "xx?" };

    const auto first =
        wincpp::patterns::scanner::find< wincpp::patterns::scanner::algorithm_t::bmh_t >( std::span< const std::uint8_t >{ bytes }, pattern );
    const auto all =
        wincpp::patterns::scanner::find_all< wincpp::patterns::scanner::algorithm_t::naive_t >( std::span< const std::uint8_t >{ bytes }, pattern );

    REQUIRE( first.has_value() );
    CHECK( *first == 1 );
    CHECK( all == std::vector< std::uintptr_t >{ 1, 6 } );
}
