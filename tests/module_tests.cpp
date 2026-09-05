#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <cctype>
#include <iterator>
#include <limits>
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

    template< typename T >
    void overwrite_image_value( T* const address, const T value )
    {
        DWORD old_protection{};
        if ( !VirtualProtect( address, sizeof( *address ), PAGE_READWRITE, &old_protection ) )
            throw wincpp::core::error::from_win32( GetLastError() );

        *address = value;

        DWORD ignored{};
        if ( !VirtualProtect( address, sizeof( *address ), old_protection, &ignored ) )
            throw wincpp::core::error::from_win32( GetLastError() );
    }

    IMAGE_EXPORT_DIRECTORY* export_directory( const HMODULE module )
    {
        auto* const image = reinterpret_cast< std::uint8_t* >( module );
        const auto* const dos_header = reinterpret_cast< const IMAGE_DOS_HEADER* >( image );
        const auto* const nt_headers = reinterpret_cast< const IMAGE_NT_HEADERS* >( image + dos_header->e_lfanew );
        const auto rva = nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress;
        return reinterpret_cast< IMAGE_EXPORT_DIRECTORY* >( image + rva );
    }

    void share_export_ordinal( const HMODULE module, const std::string_view alias_name, const std::string_view target_name )
    {
        auto* const image = reinterpret_cast< std::uint8_t* >( module );
        const auto* const directory = export_directory( module );
        const auto* const names = reinterpret_cast< const std::uint32_t* >( image + directory->AddressOfNames );
        auto* const ordinals = reinterpret_cast< std::uint16_t* >( image + directory->AddressOfNameOrdinals );

        std::uint16_t* alias_entry = nullptr;
        std::uint16_t* target_entry = nullptr;
        for ( std::uint32_t index = 0; index < directory->NumberOfNames; ++index )
        {
            const std::string_view name{ reinterpret_cast< const char* >( image + names[ index ] ) };
            if ( name == alias_name )
                alias_entry = &ordinals[ index ];
            if ( name == target_name )
                target_entry = &ordinals[ index ];
        }

        if ( !alias_entry || !target_entry )
            throw wincpp::core::error::from_user( wincpp::core::user_error_type_t::export_not_found_t, "Test fixture export is missing" );

        overwrite_image_value( alias_entry, *target_entry );
    }

    void narrow_export_directory( const HMODULE module )
    {
        auto* const image = reinterpret_cast< std::uint8_t* >( module );
        const auto* const dos_header = reinterpret_cast< const IMAGE_DOS_HEADER* >( image );
        auto* const nt_headers = reinterpret_cast< IMAGE_NT_HEADERS* >( image + dos_header->e_lfanew );
        auto* const size = &nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].Size;
        overwrite_image_value( size, static_cast< DWORD >( sizeof( IMAGE_EXPORT_DIRECTORY ) ) );
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

    const auto symbol_iterator = std::ranges::find_if( exports, []( const auto& exp ) { return exp != nullptr && !exp->name().empty(); } );
    REQUIRE( symbol_iterator != exports.end() );
    const auto& symbol = *symbol_iterator;

    CHECK_FALSE( symbol->name().empty() );
    CHECK( symbol->address() >= ( *ntdll )->address() );
    CHECK( symbol->module() != nullptr );
    CHECK_FALSE( symbol->to_string().empty() );

    CHECK( ( *ntdll )->fetch_export( symbol->name() ) != nullptr );
    const auto symbol_result = ( *ntdll )->try_fetch_export( symbol->name() );
    REQUIRE( symbol_result.has_value() );
    REQUIRE( *symbol_result != nullptr );
    CHECK( ( *symbol_result )->address() == symbol->address() );

    const auto ordinal_symbol = ( *ntdll )->fetch_export( symbol->ordinal() );
    REQUIRE( ordinal_symbol != nullptr );
    CHECK( ordinal_symbol->address() == symbol->address() );

    const auto ordinal_result = ( *ntdll )->try_fetch_export( symbol->ordinal() );
    REQUIRE( ordinal_result.has_value() );
    CHECK( ( *ordinal_result )->address() == symbol->address() );

    const auto missing_result = ( *ntdll )->try_fetch_export( "__missing_export__" );
    REQUIRE_FALSE( missing_result.has_value() );
    CHECK( missing_result.error().code().message() == "The desired export was not found." );

    CHECK( ( *ntdll )->fetch_export( "__missing_export__" ) == nullptr );
    CHECK( ( *ntdll )->fetch_export( std::numeric_limits< std::uint32_t >::max() ) == nullptr );
    CHECK_THROWS_AS( ( **ntdll )[ "__missing_export__" ], wincpp::core::error );
}

TEST_CASE( "Kernel32 API-set revision forwarders agree with the Windows loader", "[modules][exports][api-set]" )
{
    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto kernel32 = process->module_factory.fetch_module( "kernel32.dll" );
    REQUIRE( kernel32 != nullptr );

    const auto expected = GetProcAddress( GetModuleHandleW( L"kernel32.dll" ), "InitOnceExecuteOnce" );
    REQUIRE( expected != nullptr );
    const auto symbol = kernel32->fetch_export( "InitOnceExecuteOnce" );
    REQUIRE( symbol != nullptr );
    CHECK( symbol->address() == reinterpret_cast< std::uintptr_t >( expected ) );

    const auto host = process->module_factory.fetch_module( "API-MS-WIN-CORE-SYNCH-L1-2-0.DLL" );
    REQUIRE( host != nullptr );
    const auto host_symbol = host->fetch_export( "InitOnceExecuteOnce" );
    REQUIRE( host_symbol != nullptr );
    CHECK( host_symbol->address() == reinterpret_cast< std::uintptr_t >( expected ) );
}

TEST_CASE( "Export parsing handles mapped tables, ordinals, and forwarders", "[modules][exports]" )
{
    const auto target_image = LoadLibraryA( "wincpp_export_target.dll" );
    const auto forwarder_image = LoadLibraryA( "wincpp_export_forwarders.dll" );
    REQUIRE( target_image != nullptr );
    REQUIRE( forwarder_image != nullptr );
    REQUIRE_NOTHROW( share_export_ordinal( target_image, "NamedAlias", "NamedExport" ) );
    REQUIRE_NOTHROW( narrow_export_directory( target_image ) );

    const auto process = wincpp::process_t::current();
    REQUIRE( process != nullptr );

    const auto target = process->module_factory.fetch_module( "WINCPP_EXPORT_TARGET" );
    const auto forwarders = process->module_factory.fetch_module( "wincpp_export_forwarders.dll" );
    REQUIRE( target != nullptr );
    REQUIRE( forwarders != nullptr );

    const auto named = target->fetch_export( "NamedExport" );
    const auto alias = target->fetch_export( "NamedAlias" );
    const auto ordinal_only = target->fetch_export( 9 );
    REQUIRE( named != nullptr );
    REQUIRE( alias != nullptr );
    REQUIRE( ordinal_only != nullptr );
    CHECK( named->ordinal() == 7 );
    CHECK( alias->ordinal() == named->ordinal() );
    CHECK( alias->address() == named->address() );
    CHECK( alias->address() == reinterpret_cast< std::uintptr_t >( GetProcAddress( target_image, "NamedAlias" ) ) );
    CHECK( ordinal_only->name().empty() );
    CHECK( ordinal_only->address() == named->address() );
    CHECK( ordinal_only->to_string().contains( "#9" ) );

    const auto& target_exports = target->exports();
    CHECK( std::ranges::count_if( target_exports, []( const auto& exp ) { return exp->ordinal() == 7 && !exp->name().empty(); } ) == 2 );
    CHECK( std::ranges::any_of( target_exports, []( const auto& exp ) { return exp->ordinal() == 8 && exp->name().empty(); } ) );
    CHECK( std::ranges::any_of( target_exports, []( const auto& exp ) { return exp->ordinal() == 9 && exp->name().empty(); } ) );

    const auto by_name = forwarders->fetch_export( "ForwardedByName" );
    const auto by_ordinal = forwarders->fetch_export( "ForwardedByOrdinal" );
    REQUIRE( by_name != nullptr );
    REQUIRE( by_ordinal != nullptr );
    CHECK( by_name->module()->address() == target->address() );
    CHECK( by_ordinal->module()->address() == target->address() );
    CHECK( by_name->address() == named->address() );
    CHECK( by_ordinal->address() == ordinal_only->address() );
    CHECK( by_name->address() == reinterpret_cast< std::uintptr_t >( GetProcAddress( forwarder_image, "ForwardedByName" ) ) );
    CHECK( by_ordinal->address() == reinterpret_cast< std::uintptr_t >( GetProcAddress( forwarder_image, "ForwardedByOrdinal" ) ) );

    const auto missing_module_forwarder = forwarders->try_fetch_export( "MissingModuleForwarder" );
    REQUIRE_FALSE( missing_module_forwarder.has_value() );
    CHECK( missing_module_forwarder.error().code().message() == "The desired module was not found." );
    CHECK_THROWS_AS( forwarders->exports(), wincpp::core::error );

    const auto missing_export_forwarder = forwarders->try_fetch_export( "MissingExportForwarder" );
    REQUIRE_FALSE( missing_export_forwarder.has_value() );
    CHECK( missing_export_forwarder.error().code().message() == "The desired export was not found." );

    const auto circular_forwarder = forwarders->try_fetch_export( "CircularForwarder" );
    REQUIRE_FALSE( circular_forwarder.has_value() );
    CHECK( circular_forwarder.error().code().message() == "The requested operation failed." );

    const auto api_set = forwarders->fetch_export( "ApiSetForwarder" );
    REQUIRE( api_set != nullptr );
    REQUIRE( api_set->module() != nullptr );
    CHECK( api_set->address() >= api_set->module()->address() );
    CHECK( api_set->address() == reinterpret_cast< std::uintptr_t >( GetProcAddress( forwarder_image, "ApiSetForwarder" ) ) );
}
