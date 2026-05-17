#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <iterator>
#include <string>
#include <wincpp/wincpp.hpp>

namespace
{
    std::string lowercase( const std::string_view value )
    {
        std::string result;
        result.reserve( value.size() );

        std::ranges::transform(
            value, std::back_inserter( result ), []( const unsigned char ch ) { return static_cast< char >( std::tolower( ch ) ); } );

        return result;
    }
}  // namespace

TEST_CASE( "Module factory exposes the current process modules", "[modules]" )
{
    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto& modules = process->module_factory.modules();
    REQUIRE_FALSE( modules.empty() );

    const auto& main_module = process->module_factory.main_module();

    CHECK( main_module.address() != 0 );
    CHECK( main_module.size() > 0 );
    CHECK_FALSE( main_module.name().empty() );
    CHECK_FALSE( main_module.path().empty() );

    const auto fetched = process->module_factory.fetch_module( main_module.name() );
    REQUIRE( fetched != nullptr );
    CHECK( fetched->address() == main_module.address() );

    const auto fetched_result = process->module_factory.try_fetch_module( main_module.name() );
    REQUIRE( fetched_result.has_value() );
    REQUIRE( *fetched_result != nullptr );
    CHECK( ( *fetched_result )->address() == main_module.address() );

    const auto missing_result = process->module_factory.try_fetch_module( "__missing_module__.dll" );
    REQUIRE_FALSE( missing_result.has_value() );
    CHECK( missing_result.error().code().message() == "The desired module was not found." );

    process->module_factory.refresh();
    CHECK_FALSE( process->module_factory.modules().empty() );

    CHECK_THROWS_AS( process->module_factory[ "__missing_module__.dll" ], wincpp::core::error );
}

TEST_CASE( "Module sections and RTTI lookup are queryable", "[modules][sections]" )
{
    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto& main_module = process->module_factory.main_module();
    const auto& sections = main_module.sections();

    REQUIRE_FALSE( sections.empty() );
    CHECK( std::ranges::all_of( sections, []( const auto& section ) { return section != nullptr && section->size() > 0; } ) );

    if ( const auto text = main_module.fetch_section( ".text" ) )
    {
        CHECK( text->address() >= main_module.address() );
        CHECK_FALSE( text->name().empty() );
    }

    const auto objects = main_module.fetch_objects( ".?AVdefinitely_missing_wincpp_type@@" );
    CHECK( objects.empty() );
}

TEST_CASE( "Export wrappers expose symbol metadata when an exporting module is present", "[modules][exports]" )
{
    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto& modules = process->module_factory.modules();
    const auto ntdll = std::ranges::find_if( modules, []( const auto& module ) { return lowercase( module->name() ) == "ntdll.dll"; } );

    REQUIRE( ntdll != modules.end() );

    const auto& exports = ( *ntdll )->exports();
    REQUIRE_FALSE( exports.empty() );

    const auto symbol = exports.front();
    REQUIRE( symbol != nullptr );

    CHECK_FALSE( symbol->name().empty() );
    CHECK( symbol->address() >= ( *ntdll )->address() );
    CHECK( symbol->module() != nullptr );
    CHECK_FALSE( symbol->to_string().empty() );

    CHECK( ( *ntdll )->fetch_export( symbol->name() ) != nullptr );
    const auto symbol_result = ( *ntdll )->try_fetch_export( symbol->name() );
    REQUIRE( symbol_result.has_value() );
    REQUIRE( *symbol_result != nullptr );
    CHECK( ( *symbol_result )->address() == symbol->address() );

    const auto missing_result = ( *ntdll )->try_fetch_export( "__missing_export__" );
    REQUIRE_FALSE( missing_result.has_value() );
    CHECK( missing_result.error().code().message() == "The desired export was not found." );

    CHECK( ( *ntdll )->fetch_export( "__missing_export__" ) == nullptr );
    CHECK_THROWS_AS( ( **ntdll )[ "__missing_export__" ], wincpp::core::error );
}
