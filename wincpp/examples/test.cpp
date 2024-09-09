#include <iostream>

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

        const auto& load_library = process->module_factory[ "kernel32.dll" ][ "LoadLibraryA" ];

        std::cout << load_library << std::endl;
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}