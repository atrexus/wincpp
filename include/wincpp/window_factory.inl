#pragma once

#include "wincpp/detail/win32.hpp"
#include "wincpp/process.hpp"
#include "wincpp/window_factory.hpp"
#include "wincpp/windows/window.hpp"

namespace wincpp
{
    namespace detail
    {
        struct enum_windows_data final
        {
            std::vector< windows::window_t >& windows;
            process_t& process;
        };
    }  // namespace detail

    inline window_factory::window_factory( process_t* const p ) noexcept : p( p )
    {
    }

    inline std::optional< windows::window_t > window_factory::main_window() const
    {
        for ( const auto& window : windows() )
        {
            if ( window.owner() == std::nullopt )
                return window;
        }

        return std::nullopt;
    }

    inline std::vector< windows::window_t > window_factory::windows() const
    {
        std::vector< windows::window_t > result;
        auto data = detail::enum_windows_data{ result, *p };

        const auto callback = []( const HWND hwnd, const LPARAM param ) -> BOOL
        {
            auto& [ windows, process ] = *reinterpret_cast< detail::enum_windows_data* >( param );

            DWORD pid{};
            GetWindowThreadProcessId( hwnd, &pid );

            if ( process.id() == pid && IsWindowVisible( hwnd ) )
                windows.emplace_back( process.thread_factory, hwnd );

            return TRUE;
        };

        EnumWindows( callback, reinterpret_cast< LPARAM >( &data ) );
        return result;
    }
}  // namespace wincpp
