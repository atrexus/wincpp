#include <iostream>
#include <chrono>
#include <thread>

#include "process.hpp"


using namespace wincpp;

int main()
{
    try
    {
        const auto process = process_t::open( "Notepad.exe" );

        if ( !process )
        {
            std::cout << "Failed to open the process." << std::endl;
            return 1;
        }

        if ( const auto& op = process->memory_factory.protect( 0x24796557000, 100, memory::protection_flags_t::execute_readwrite ) )
        {
            std::cout << "Successfully changed the protection of the memory." << std::endl;
            std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
        }

        std::cout << "Resetting the protection of the memory." << std::endl;
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}