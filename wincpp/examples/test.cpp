#include <chrono>
#include <iostream>
#include <thread>

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

        const auto rdata = process->module_factory.main_module().fetch_section( ".rdata" );

        std::cout << std::hex << rdata->address() << std::endl;
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}