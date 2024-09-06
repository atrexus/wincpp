#include <iostream>

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

        for ( const auto& region : process->memory_factory.regions() )
        {
            std::cout << "0x" << std::hex << region.address() << " (" << region.protection() << ")" << std::endl;
        }
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}