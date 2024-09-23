#include "core/win.hpp"

namespace wincpp::core
{
    std::shared_ptr< handle_t > handle_t::create( HANDLE hHandle, bool owns_handle )
    {
        return std::shared_ptr< handle_t >( new handle_t( hHandle, owns_handle ), deleter() );
    }

    handle_t::handle_t( HANDLE hHandle, bool owns_handle ) : native( hHandle ), owns_handle( owns_handle )
    {
    }

    void handle_t::deleter::operator()( handle_t* handle ) const
    {
        if ( !handle )
            return;

        if ( handle->owns_handle && handle->native != INVALID_HANDLE_VALUE )
        {
            CloseHandle( handle->native );
        }

        delete handle;
    }

    rectangle_t::rectangle_t( const RECT& rect ) noexcept : left( rect.left ), top( rect.top ), right( rect.right ), bottom( rect.bottom )
    {
    }

    point_t::point_t( const POINT& point ) noexcept : x( point.x ), y( point.y )
    {
    }
}  // namespace wincpp::core