#pragma once

#include "wincpp/core/win.hpp"
#include "wincpp/detail/win32.hpp"

namespace wincpp::core
{
    constexpr process_access_t operator|( const process_access_t lhs, const process_access_t rhs ) noexcept
    {
        // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
        return static_cast< process_access_t >( std::to_underlying( lhs ) | std::to_underlying( rhs ) );
    }

    constexpr process_access_t operator&( const process_access_t lhs, const process_access_t rhs ) noexcept
    {
        // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
        return static_cast< process_access_t >( std::to_underlying( lhs ) & std::to_underlying( rhs ) );
    }

    constexpr process_access_t& operator|=( process_access_t& lhs, const process_access_t rhs ) noexcept
    {
        lhs = lhs | rhs;
        return lhs;
    }

    constexpr bool has_flag( const process_access_t value, const process_access_t flags ) noexcept
    {
        return ( std::to_underlying( value ) & std::to_underlying( flags ) ) == std::to_underlying( flags );
    }

    inline std::shared_ptr< handle_t > handle_t::create( const native_handle_t handle, const bool owns_handle )
    {
        return { new handle_t( handle, owns_handle ), deleter{} };
    }

    inline handle_t::handle_t( const native_handle_t handle, const bool owns_handle ) : native( handle ), owns_handle( owns_handle )
    {
    }

    inline void handle_t::deleter::operator()( handle_t* const handle ) const
    {
        if ( handle )
        {
            const auto native = static_cast< HANDLE >( handle->native );

            if ( handle->owns_handle && native && native != INVALID_HANDLE_VALUE )
                CloseHandle( native );
        }

        delete handle;
    }

    inline rectangle_t::rectangle_t() noexcept : rectangle_t( 0, 0, 0, 0 )
    {
    }

    inline rectangle_t::rectangle_t( const std::int32_t left, const std::int32_t top, const std::int32_t right, const std::int32_t bottom ) noexcept
        : left( left ),
          top( top ),
          right( right ),
          bottom( bottom )
    {
    }

    constexpr std::int32_t rectangle_t::height() const noexcept
    {
        return bottom - top;
    }

    constexpr void rectangle_t::set_height( const std::int32_t value ) noexcept
    {
        bottom = top + value;
    }

    constexpr std::int32_t rectangle_t::width() const noexcept
    {
        return right - left;
    }

    constexpr void rectangle_t::set_width( const std::int32_t value ) noexcept
    {
        right = left + value;
    }

    inline point_t::point_t() noexcept : point_t( 0, 0 )
    {
    }

    inline point_t::point_t( const std::int32_t x, const std::int32_t y ) noexcept : x( x ), y( y )
    {
    }
}  // namespace wincpp::core
