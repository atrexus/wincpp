#include <limits>
#include <wincpp/wincpp.hpp>

#ifdef min
#error "wincpp must not leak the Windows min macro"
#endif

#ifdef max
#error "wincpp must not leak the Windows max macro"
#endif

#include <catch2/catch_test_macros.hpp>

TEST_CASE( "Aggregate header leaves numeric limits usable", "[headers][macro-hygiene]" )
{
    CHECK( std::numeric_limits< int >::max() > 0 );
    CHECK( ( std::numeric_limits< std::size_t >::max )() > 0 );
}
