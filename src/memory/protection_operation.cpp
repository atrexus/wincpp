#include "memory/protection_operation.hpp"

#include "process.hpp"

namespace wincpp::memory
{
    protection_operation_t::protection_operation_t(
        std::uintptr_t address,
        std::size_t size,
        protection_flags_t new_flags,
        protection_flags_t old_flags ) noexcept
        : address( address ),
          size( size ),
          new_flags( new_flags ),
          old_flags( old_flags )
    {
    }

    protection_operation_t::deleter::deleter( std::shared_ptr< core::handle_t > handle ) noexcept : handle( handle )
    {
    }

    void protection_operation_t::deleter::operator()( protection_operation_t* operation ) const
    {
        DWORD old_flags;
        if ( !VirtualProtectEx(
                 handle->native, reinterpret_cast< void* >( operation->address ), operation->size, operation->old_flags.get(), &old_flags ) )
            throw core::error::from_win32( GetLastError() );

        delete operation;
    }

}  // namespace wincpp::memory