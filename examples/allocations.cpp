#include <array>
#include <cstddef>
#include <exception>
#include <print>
#include <span>
#include <system_error>
#include <wincpp/wincpp.hpp>

using namespace wincpp;

int main()
{
    try
    {
        const auto process = process_t::current();
        const auto allocation = process->memory_factory.allocate( 512, memory::protection_flags_t::readwrite );

        process->memory_factory.write_string( allocation->address(), "hello from wincpp" );
        process->memory_factory.write_wstring( allocation->address() + 64, L"wide text" );

        constexpr auto marker = std::array{
            std::byte{ 0xDE },
            std::byte{ 0xAD },
            std::byte{ 0xBE },
            std::byte{ 0xEF },
        };

        allocation->write( 128, std::span< const std::byte >{ marker } );

        std::array< std::byte, marker.size() > read_back{};
        allocation->read( 128, std::span< std::byte >{ read_back } );

        const auto pattern = patterns::pattern_t::from_ida( "DE AD BE EF" );
        const auto found = allocation->find( pattern, patterns::scanner::algorithm_t::bmh_t );
        const auto region = process->memory_factory.query( allocation->address() );

        std::println( "process: {} ({})", process->name(), process->id() );
        std::println( "allocation: 0x{:X} ({} bytes)", allocation->address(), allocation->size() );
        std::println( "narrow string: {}", process->memory_factory.read_string( allocation->address(), 64 ) );
        std::println( "wide string length: {}", process->memory_factory.read_wstring( allocation->address() + 64, 32 ).size() );
        std::println( "marker found at: 0x{:X}", found.value_or( 0 ) );

        if ( region )
            std::println(
                "region: 0x{:X} ({} bytes, committed: {})",
                region->address(),
                region->size(),
                region->state() == memory::region_t::state_t::commit_t );
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
