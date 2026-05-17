#include <algorithm>
#include <exception>
#include <print>
#include <ranges>
#include <string_view>
#include <system_error>
#include <wincpp/wincpp.hpp>

using namespace wincpp;

int main()
{
    try
    {
        const auto process = process_t::current();
        const auto reopened =
            process_t::try_open( process->id(), core::process_access_t::query_limited_information_t | core::process_access_t::synchronize_t );

        std::println( "current process: {} ({})", process->name(), process->id() );
        std::println( "limited reopen: {}", reopened ? "success" : reopened.error().what() );

        const auto& modules = process->module_factory.modules( true );
        const auto& main_module = process->module_factory.main_module();

        std::println( "modules loaded: {}", modules.size() );
        std::println( "main module: {} @ 0x{:X}", main_module.name(), main_module.address() );
        std::println( "main module path: {}", main_module.path() );

        std::println( "\nfirst sections:" );
        for ( const auto& section : main_module.sections() | std::views::take( 5 ) )
        {
            std::println( "  {:<8} 0x{:X} ({} bytes)", section->name(), section->address(), section->size() );
        }

        const auto ntdll = process->module_factory.try_fetch_module( "ntdll.dll" );

        if ( !ntdll )
        {
            std::println( "\nntdll.dll was not found in the current process." );
            return 0;
        }

        const auto& exports = ( *ntdll )->exports();

        std::println( "\n{} exports: {}", ( *ntdll )->name(), exports.size() );

        for ( const auto& exp : exports | std::views::take( 8 ) )
        {
            std::println( "  {}", exp->to_string() );
        }

        const auto nt_close = ( *ntdll )->try_fetch_export( "NtClose" );
        std::println( "\nNtClose lookup: {}", nt_close ? ( *nt_close )->to_string() : nt_close.error().what() );
    }
    catch ( const std::system_error& e )
    {
        std::println( "[-] Error [{}]: {}", e.code().value(), e.what() );
        return 1;
    }
    catch ( const std::exception& e )
    {
        std::println( "[-] Error: {}", e.what() );
        return 1;
    }

    return 0;
}
