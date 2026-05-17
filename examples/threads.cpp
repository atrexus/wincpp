#include <atomic>
#include <chrono>
#include <exception>
#include <print>
#include <system_error>
#include <thread>
#include <wincpp/wincpp.hpp>

using namespace std::chrono_literals;
using namespace wincpp;

int main()
{
    try
    {
        std::atomic_size_t ticks{};
        std::jthread worker(
            [ & ]( const std::stop_token& stop )
            {
                while ( !stop.stop_requested() )
                {
                    ++ticks;
                    std::this_thread::sleep_for( 5ms );
                }
            } );

        const auto process = process_t::current();
        const auto threads = process->thread_factory.threads().vector();

        std::println( "current process: {} ({})", process->name(), process->id() );
        std::println( "threads: {}", threads.size() );

        for ( const auto& thread : threads )
        {
            std::println( "  thread {} owned by process {}", thread.id(), thread.process_id() );
        }

        std::this_thread::sleep_for( 50ms );
        const auto before_suspend = ticks.load();

        {
            const auto guard = process->thread_factory.scoped_suspend_all();

            std::this_thread::sleep_for( 50ms );

            std::println( "scoped suspend guard owns {} suspended thread(s)", guard.size() );
        }

        std::this_thread::sleep_for( 50ms );
        std::println( "worker ticks before/after guard: {} -> {}", before_suspend, ticks.load() );
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
