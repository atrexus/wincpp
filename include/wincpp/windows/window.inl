#pragma once

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/windows/window.hpp"

namespace wincpp::windows
{
    inline window_t::window_t( const thread_factory& tds, const core::native_window_handle_t hwnd ) noexcept : hwnd( hwnd ), tds( tds )
    {
    }

    inline core::native_window_handle_t window_t::handle() const noexcept
    {
        return hwnd;
    }

    inline std::uint32_t window_t::process_id() const
    {
        DWORD pid{};

        if ( !GetWindowThreadProcessId( static_cast< HWND >( hwnd ), &pid ) )
            throw core::error::from_win32( GetLastError() );

        return pid;
    }

    inline std::optional< window_t > window_t::owner() const noexcept
    {
        const auto owner = GetWindow( static_cast< HWND >( hwnd ), GW_OWNER );

        if ( !owner )
            return std::nullopt;

        return window_t( tds, owner );
    }

    inline std::string window_t::title() const
    {
        std::string title;
        title.resize( static_cast< std::size_t >( GetWindowTextLengthA( static_cast< HWND >( hwnd ) ) ) + 1 );

        if ( !GetWindowTextA( static_cast< HWND >( hwnd ), title.data(), static_cast< int >( title.size() ) ) && GetLastError() )
            throw core::error::from_win32( GetLastError() );

        title.resize( std::char_traits< char >::length( title.c_str() ) );
        return title;
    }

    inline std::string window_t::class_name() const
    {
        std::string class_name;
        class_name.resize( 256 );

        if ( !GetClassNameA( static_cast< HWND >( hwnd ), class_name.data(), static_cast< int >( class_name.size() ) ) )
            throw core::error::from_win32( GetLastError() );

        class_name.resize( std::char_traits< char >::length( class_name.c_str() ) );
        return class_name;
    }

    inline bool window_t::is_active() const
    {
        return GetForegroundWindow() == static_cast< HWND >( hwnd );
    }

    inline threads::thread_t window_t::thread() const
    {
        if ( const auto tid = GetWindowThreadProcessId( static_cast< HWND >( hwnd ), nullptr ) )
            return tds[ tid ];

        throw core::error::from_win32( GetLastError() );
    }

    inline window_t::placement_t window_t::placement() const
    {
        WINDOWPLACEMENT placement{};
        placement.length = sizeof( placement );

        if ( !GetWindowPlacement( static_cast< HWND >( hwnd ), &placement ) )
            throw core::error::from_win32( GetLastError() );

        return {
            placement.flags,
            static_cast< window_t::state_t >( placement.showCmd ),
            core::point_t( placement.ptMinPosition.x, placement.ptMinPosition.y ),
            core::point_t( placement.ptMaxPosition.x, placement.ptMaxPosition.y ),
            core::rectangle_t(
                placement.rcNormalPosition.left, placement.rcNormalPosition.top, placement.rcNormalPosition.right, placement.rcNormalPosition.bottom )
        };
    }

    inline window_t::placement_t::placement_t(
        const std::uint32_t flags,
        const state_t show_state,
        const core::point_t min_position,
        const core::point_t max_position,
        const core::rectangle_t normal_position ) noexcept
        : flags( flags ),
          show_state( show_state ),
          min_position( min_position ),
          max_position( max_position ),
          normal_position( normal_position )
    {
    }
}  // namespace wincpp::windows
