#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <string_view>
#include <type_traits>
#include <wincpp/detail/api_set.hpp>

namespace
{
    struct api_set_test_schema_t
    {
        wincpp::detail::api_set_namespace_t header{};
        wincpp::detail::api_set_namespace_entry_t entry{};
        wincpp::detail::api_set_value_entry_t values[ 2 ]{};
        wchar_t name[ 64 ] = L"API-MS-WIN-CORE-SYNCH-L1-2-1";
        wchar_t alias[ 32 ] = L"Kernel32.dll";
        wchar_t alias_host[ 32 ] = L"SynchAlias.dll";
        wchar_t default_host[ 32 ] = L"KernelBase.dll";

        api_set_test_schema_t()
        {
            const auto byte_length = []( const wchar_t* value )
            { return static_cast< std::uint32_t >( std::wstring_view( value ).size() * sizeof( wchar_t ) ); };

            header = { 6, sizeof( *this ), 0, 1, offsetof( api_set_test_schema_t, entry ), 0, 0 };
            entry = { 0, offsetof( api_set_test_schema_t, name ), byte_length( name ), 52, offsetof( api_set_test_schema_t, values ), 2 };
            values[ 0 ] = { 0,
                            offsetof( api_set_test_schema_t, alias ),
                            byte_length( alias ),
                            offsetof( api_set_test_schema_t, alias_host ),
                            byte_length( alias_host ) };
            values[ 1 ] = { 0, 0, 0, offsetof( api_set_test_schema_t, default_host ), byte_length( default_host ) };
        }
    };

    struct api_set_test_memory_t
    {
        api_set_test_schema_t schema;

        template< typename T >
        T read( const std::uintptr_t ) const
        {
            // Redirect the PEB's map read to the synthetic schema without changing the live PEB.
            if constexpr ( std::is_same_v< T, std::uintptr_t > )
                return reinterpret_cast< std::uintptr_t >( &schema );
            else
            {
                static_assert( std::is_same_v< T, wincpp::detail::api_set_namespace_t > );
                return schema.header;
            }
        }

        std::unique_ptr< std::uint8_t[] > read( const std::uintptr_t address, const std::size_t size ) const
        {
            REQUIRE( address == reinterpret_cast< std::uintptr_t >( &schema ) );
            REQUIRE( size == sizeof( schema ) );
            auto buffer = std::make_unique< std::uint8_t[] >( size );
            std::memcpy( buffer.get(), &schema, size );
            return buffer;
        }
    };
}  // namespace

TEST_CASE( "API-set lookup accepts contract revisions in the same hashed family", "[modules][api-set]" )
{
    const api_set_test_memory_t memory;

    for ( const auto contract :
          { "api-ms-win-core-synch-l1-2-0", "api-ms-win-core-synch-l1-2-1", "api-ms-win-core-synch-l1-2-999", "API-MS-WIN-CORE-SYNCH-L1-2-0.DLL" } )
    {
        CAPTURE( contract );
        CHECK( wincpp::detail::api_set_host( memory, GetCurrentProcess(), contract ) == "kernelbase.dll" );
    }
}

TEST_CASE( "API-set lookup keeps distinct hashed families separate", "[modules][api-set]" )
{
    const api_set_test_memory_t memory;

    for ( const auto contract : { "api-ms-win-core-synch-l1-20-0",
                                  "api-ms-win-core-synch-l1-3-0",
                                  "api-ms-win-core-synch-l1-2-0-extra",
                                  "api-ms-win-core-synch-l1-2",
                                  "api-ms-win-core-synch",
                                  "api-",
                                  "kernel32.dll" } )
    {
        CAPTURE( contract );
        CHECK( wincpp::detail::api_set_host( memory, GetCurrentProcess(), contract ).empty() );
    }
}

TEST_CASE( "API-set lookup rejects malformed hashed lengths", "[modules][api-set]" )
{
    api_set_test_memory_t memory;

    for ( const auto hashed_length :
          { std::uint32_t{ 0 }, std::uint32_t{ 51 }, memory.schema.entry.name_length + 2, std::numeric_limits< std::uint32_t >::max() } )
    {
        CAPTURE( hashed_length );
        memory.schema.entry.hashed_length = hashed_length;
        CHECK( wincpp::detail::api_set_host( memory, GetCurrentProcess(), "api-ms-win-core-synch-l1-2-1" ).empty() );
    }
}

TEST_CASE( "API-set revision lookup preserves importing-module aliases and fallback hosts", "[modules][api-set]" )
{
    api_set_test_memory_t memory;
    constexpr auto contract = "api-ms-win-core-synch-l1-2-0";

    CHECK( wincpp::detail::api_set_host( memory, GetCurrentProcess(), contract, "KERNEL32.DLL" ) == "synchalias.dll" );
    CHECK( wincpp::detail::api_set_host( memory, GetCurrentProcess(), contract, "other.dll" ) == "kernelbase.dll" );

    memory.schema.entry.value_count = 1;
    CHECK( wincpp::detail::api_set_host( memory, GetCurrentProcess(), contract ) == "synchalias.dll" );
}
