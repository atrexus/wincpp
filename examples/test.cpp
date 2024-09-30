#include <chrono>
#include <execution>
#include <iostream>
#include <thread>
#include <wincpp/patterns/scanner.hpp>
#include <wincpp/process.hpp>

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
        const auto start = std::chrono::high_resolution_clock::now();
        const auto objects = main_module.fetch_objects( ".?AVDataModel@RBX@@" );

        if ( objects.empty() )
        {
            std::cout << "Failed to find the object." << std::endl;
            return 1;
        }

        const auto& object = objects[ 4 ];
        const auto address = process->memory_factory.find_instance_of(
            object, []( const memory::region_t& region ) -> bool { return region.size() != 0x200000; }, false );

        if ( !address )
        {
            std::cout << "Failed to find the object." << std::endl;
            return 1;
        }

        const auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Found object " << object->name() << "<" << std::hex << object->vtable() << ">"
                  << " at 0x" << std::hex << *address << " in " << std::dec << std::chrono::duration_cast< std::chrono::milliseconds >( end - start )
                  << " ms" << std::endl;

        // Get the first memory region and print its working set information
        // for ( const auto& region : process->memory_factory.regions() )
        //{
        //    const auto info = region.working_set_information();

        //    std::cout << "Virtual Address: " << std::hex << info.virtual_address << std::endl;
        //    std::cout << "\tValid: " << info.valid << std::endl;
        //    std::cout << "\tShare Count: " << info.share_count << std::endl;
        //    std::cout << "\tProtection: " << info.protection << std::endl;
        //}
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