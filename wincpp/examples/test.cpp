#include <chrono>
#include <iostream>
#include <thread>

#include "patterns/scanner.hpp"
#include "process.hpp"

using namespace wincpp;

int main()
{
    try
    {
        const auto process = process_t::open( "RobloxPlayerBeta.exe" );

        if ( !process )
        {
            std::cout << "Failed to open the process." << std::endl;
            return 1;
        }

        const auto& main_module = process->module_factory.main_module();

        const auto objects = main_module.fetch_objects( ".?AVDataModel@RBX@@" );
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