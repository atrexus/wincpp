#include <chrono>
#include <iostream>
#include <thread>

#include "patterns/pattern.hpp"
#include "process.hpp"

using namespace wincpp;

int main()
{
    try
    {
        const auto process = process_t::current();

        if ( !process )
        {
            std::cout << "Failed to open the process." << std::endl;
            return 1;
        }

        const auto pattern = patterns::pattern_t( "48 8B 05 ? ? ? ? 48 8B 0C C8 48 8B 04 D1 48 8B 04 D1" );

        const auto pattern2 = patterns::pattern_t( "\x48\x8B\x05\x00\x00\x00\x00\x45", "xxx????x" );

        const auto pattern3 = patterns::pattern_t::from< std::string >( ".?AVDataModel@RBX@@" );

        std::cout << pattern3 << std::endl;
    }
    catch ( const std::system_error& e )
    {
        std::cout << "[-] Error [" << e.code() << "]: " << e.what() << std::endl;
    }
    catch ( const std::exception& e )
    {
        std::cout << "[-] Error: " << e.what() << std::endl;
    }

    return 0;
}