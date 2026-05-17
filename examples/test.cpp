#include <cstdint>
#include <cstring>
#include <exception>
#include <iostream>
#include <system_error>
#include <wincpp/patterns/pattern.hpp>
#include <wincpp/process.hpp>

using namespace wincpp;

int main()
{
    try
    {
        const auto process = process_t::open( "RobloxPlayerBeta.exe" );

        if ( !process )
        {
            std::cout << "Failed to open the process.\n";
            return 1;
        }

        const auto& hyp = process->module_factory[ "RobloxPlayerBeta.dll" ];

        // 48 8D 0D ? ? ? ? 48 8D 55 F8 -> lea rcx, [rel data_????????]
        const auto address = hyp.find( patterns::pattern_t{ "\x48\x8D\x0D\x00\x00\x00\x00\x48\x8D\x55\xF8", "xxx????xxxx" } );

        if ( !address )
        {
            std::cout << "Failed to find the pattern.\n";
            return 1;
        }

        std::cout << "Found the pattern at: 0x" << std::hex << *address << '\n';

        const auto instruction = hyp.read( *address, 7 );

        std::int32_t relative_offset{};
        std::memcpy( &relative_offset, instruction.get() + 3, sizeof( relative_offset ) );
        const auto data_address = *address + 7 + relative_offset;

        std::cout << "The data address is: 0x" << std::hex << data_address << ", 0x" << data_address - hyp.address() << '\n';

        const auto& kernel32 = process->module_factory[ "kernel32.dll" ];

        std::cout << "0x" << kernel32.address() << '\n';
    }
    catch ( const std::system_error& e )
    {
        std::cout << "[-] Error [" << e.code() << "]: " << e.what() << '\n';
    }
    catch ( const std::exception& e )
    {
        std::cout << "[-] Error: " << e.what() << '\n';
    }

    return 0;
}
