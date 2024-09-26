#include "wincpp/memory/region.hpp"

#include "wincpp/core/error.hpp"
#include "wincpp/process.hpp"

namespace wincpp::memory
{
    region_t::region_t( process_t *process, MEMORY_BASIC_INFORMATION mbi )
        : memory_t( process->memory_factory, reinterpret_cast< std::uintptr_t >( mbi.BaseAddress ), mbi.RegionSize ),
          process( process ),
          mbi( mbi )
    {
    }

    region_t::state_t region_t::state() const noexcept
    {
        return static_cast< state_t >( mbi.State );
    }

    region_t::type_t region_t::type() const noexcept
    {
        return static_cast< type_t >( mbi.Type );
    }

    protection_flags_t region_t::protection() const noexcept
    {
        return protection_flags_t( mbi.Protect );
    }

    region_list::region_list( process_t *process, std::uintptr_t start, std::uintptr_t stop ) noexcept
        : process( process ),
          start( start ),
          stop( stop )
    {
    }

    region_list::iterator region_list::begin() const noexcept
    {
        return iterator( process, start );
    }

    region_list::iterator region_list::end() const noexcept
    {
        return iterator( process, stop );
    }

    region_list::iterator::iterator( process_t *process, std::uintptr_t address ) : process( process ), address( address ), mbi()
    {
        if ( address != -1 )
        {
            if ( VirtualQueryEx( process->handle->native, reinterpret_cast< LPCVOID >( address ), &mbi, sizeof( mbi ) ) == 0 )
                throw core::error::from_win32( GetLastError() );
        }
    }

    region_list::iterator &region_list::iterator::operator++() noexcept
    {
        address = reinterpret_cast< std::uintptr_t >( mbi.BaseAddress ) + mbi.RegionSize;

        if ( VirtualQueryEx( process->handle->native, reinterpret_cast< LPCVOID >( address ), &mbi, sizeof( mbi ) ) == 0 )
            address = -1;

        return *this;
    }

    region_t region_list::iterator::operator*() const noexcept
    {
        return region_t( process, mbi );
    }

    bool region_list::iterator::operator!=( const iterator &other ) const noexcept
    {
        return address != other.address;
    }

    bool region_list::iterator::operator==( const iterator &other ) const noexcept
    {
        return address == other.address;
    }

    protection_operation region_t::protect( protection_flags_t new_flags ) const
    {
        return process->memory_factory.protect( address(), size(), new_flags );
    }

}  // namespace wincpp::memory