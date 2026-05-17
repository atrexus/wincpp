#include <chrono>
#include <exception>
#include <iostream>
#include <system_error>
#include <thread>
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

        process->thread_factory.suspend_all();

        std::this_thread::sleep_for( std::chrono::seconds( 5 ) );

        process->thread_factory.resume_all();
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
