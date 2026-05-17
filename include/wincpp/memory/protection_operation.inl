#pragma once

#include <utility>

#include "wincpp/core/error.hpp"
#include "wincpp/detail/win32.hpp"
#include "wincpp/memory/protection_operation.hpp"

namespace wincpp::memory
{
    inline protection_operation_t::deleter::deleter( std::shared_ptr< core::handle_t > handle, const bool scoped ) noexcept
        : handle( std::move( handle ) ),
          scoped( scoped )
    {
    }

    inline void protection_operation_t::deleter::operator()( protection_operation_t* const operation ) const
    {
        if ( operation && scoped )
        {
            DWORD ignored{};

            if ( !VirtualProtectEx(
                     static_cast< HANDLE >( handle->native ),
                     reinterpret_cast< void* >( operation->address ),
                     operation->size,
                     operation->old_flags.get(),
                     &ignored ) )
            {
                delete operation;
                throw core::error::from_win32( GetLastError() );
            }
        }

        delete operation;
    }

    inline protection_operation_t::protection_operation_t(
        const std::uintptr_t address,
        const std::size_t size,
        const protection_flags_t new_flags,
        const protection_flags_t old_flags ) noexcept
        : new_flags( new_flags ),
          old_flags( old_flags ),
          address( address ),
          size( size )
    {
    }
}  // namespace wincpp::memory
