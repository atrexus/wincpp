#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <wincpp/wincpp.hpp>

TEST_CASE( "Protection flags support value semantics and streaming", "[memory]" )
{
    auto flags = wincpp::memory::protection_flags_t{ wincpp::memory::protection_t::readwrite_t };

    CHECK( flags.has( wincpp::memory::protection_t::readwrite_t ) );
    CHECK_FALSE( flags.has( wincpp::memory::protection_t::guard_t ) );

    flags.add( wincpp::memory::protection_t::guard_t );

    CHECK( flags.has( wincpp::memory::protection_t::readwrite_t ) );
    CHECK( flags.has( wincpp::memory::protection_t::guard_t ) );

    flags.remove( wincpp::memory::protection_t::guard_t );

    CHECK( flags == wincpp::memory::protection_flags_t::readwrite );

    std::stringstream stream;
    stream << flags;

    CHECK( stream.str() == "readwrite" );
}

TEST_CASE( "Local allocations support raw, typed, string, pointer, and region operations", "[memory]" )
{
    constexpr auto first_value = std::uint32_t{ 0x0BADC0DE };
    constexpr auto second_value = std::uint32_t{ 0x00C0FFEE };

    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto allocation = process->memory_factory.allocate( 512, wincpp::memory::protection_flags_t::readwrite );
    REQUIRE( allocation != nullptr );

    CHECK( allocation->size() == 512 );
    CHECK( allocation->contains( allocation->address() ) );
    CHECK_FALSE( allocation->contains( allocation->address() + allocation->size() ) );

    process->memory_factory.write( allocation->address(), first_value );
    CHECK( process->memory_factory.read< std::uint32_t >( allocation->address() ) == first_value );

    allocation->write( sizeof( first_value ), second_value );
    CHECK( allocation->read< std::uint32_t >( sizeof( first_value ) ) == second_value );

    process->memory_factory.write_string( allocation->address() + 32, "wincpp" );
    CHECK( process->memory_factory.read_string( allocation->address() + 32 ) == "wincpp" );
    CHECK( process->memory_factory.read_string( allocation->address() + 32, 4 ) == "winc" );

    process->memory_factory.write_wstring( allocation->address() + 48, L"wide" );
    CHECK( process->memory_factory.read_wstring( allocation->address() + 48, 16 ) == L"wide" );

    const auto source_bytes = std::array{
        std::byte{ 0xDE },
        std::byte{ 0xAD },
        std::byte{ 0xBE },
        std::byte{ 0xEF },
    };
    std::array< std::byte, source_bytes.size() > target_bytes{};

    CHECK( process->memory_factory.write( allocation->address() + 80, std::span< const std::byte >{ source_bytes } ) == source_bytes.size() );
    REQUIRE( process->memory_factory.read( allocation->address() + 80, std::span< std::byte >{ target_bytes } ) );
    CHECK( std::ranges::equal( target_bytes, source_bytes ) );

    const auto object_bytes = std::array{
        std::byte{ 0x11 },
        std::byte{ 0x22 },
        std::byte{ 0x33 },
        std::byte{ 0x44 },
    };

    CHECK( allocation->write( 96, std::span< const std::byte >{ object_bytes } ) == object_bytes.size() );
    target_bytes = {};
    allocation->read( 96, std::span< std::byte >{ target_bytes } );
    CHECK( std::ranges::equal( target_bytes, object_bytes ) );

    const auto pointer = wincpp::memory::pointer_t< std::uint32_t >( allocation->address() + 64, process->memory_factory );
    *pointer = first_value;

    CHECK( *pointer == first_value );
    CHECK( pointer.address() == allocation->address() + 64 );
    CHECK( ( pointer + 4 ).address() == allocation->address() + 68 );

    const auto operation = allocation->protect( wincpp::memory::protection_flags_t::readwrite, true );
    REQUIRE( operation != nullptr );

    const auto regions = allocation->regions();
    const auto first_region = *regions.begin();
    const auto queried_region = process->memory_factory.query( allocation->address() );

    CHECK( first_region.contains( allocation->address() ) );
    CHECK( first_region.state() == wincpp::memory::region_t::state_t::commit_t );
    CHECK( first_region.protection().has( wincpp::memory::protection_t::readwrite_t ) );
    REQUIRE( queried_region.has_value() );
    CHECK( queried_region->contains( allocation->address() ) );

    const auto found = allocation->find( wincpp::patterns::pattern_t{ first_value } );
    REQUIRE( found.has_value() );
    CHECK( *found == allocation->address() );

    const auto found_bmh = allocation->find( wincpp::patterns::pattern_t{ first_value }, wincpp::patterns::scanner::algorithm_t::bmh_t );
    REQUIRE( found_bmh.has_value() );
    CHECK( *found_bmh == allocation->address() );

    const auto filtered = allocation->find(
        wincpp::patterns::pattern_t{ first_value },
        wincpp::patterns::scanner::algorithm_t::tbm_t,
        []( const wincpp::memory::region_t& ) { return false; } );
    CHECK_FALSE( filtered.has_value() );

    const auto matches = allocation->find_all( wincpp::patterns::pattern_t{ first_value } );
    CHECK_FALSE( matches.empty() );

    const auto raita_matches = allocation->find_all( wincpp::patterns::pattern_t{ first_value }, wincpp::patterns::scanner::algorithm_t::raita_t );
    CHECK_FALSE( raita_matches.empty() );

    CHECK_THROWS_AS( allocation->read< std::uint32_t >( allocation->size() ), wincpp::core::error );
    CHECK_THROWS_AS( allocation->write( allocation->size() - 1, std::span< const std::byte >{ object_bytes } ), wincpp::core::error );
}
