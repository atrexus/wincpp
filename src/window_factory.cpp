#include "wincpp/window_factory.hpp"

#include "wincpp/process.hpp"
#include "wincpp/windows/window.hpp"

namespace wincpp
{
    window_factory::window_factory( process_t* p ) noexcept : p( p )
    {
    }

    std::optional< windows::window_t > window_factory::main_window() const
    {
        for ( const auto& window : windows() )
        {
            if ( !window.owner() )
                return window;
        }

        return std::nullopt;
    }

    std::vector< windows::window_t > window_factory::windows() const
    {
        std::vector< windows::window_t > windows;

        const auto callback = []( HWND hwnd, LPARAM param ) -> BOOL
        {
            const auto& data = *reinterpret_cast< std::pair< std::vector< windows::window_t >*, process_t* >* >( param );

            DWORD pid;
            GetWindowThreadProcessId( hwnd, &pid );

            if ( pid == data.second->id() )
                data.first->push_back( windows::window_t( hwnd ) );

            return true;
        };

        const auto data = std::pair( &windows, p );

        EnumWindows( callback, reinterpret_cast< LPARAM >( &data ) );

        return windows;
    }
}  // namespace wincpp