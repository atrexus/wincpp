#include "memory/region.hpp"

#include "core/error.hpp"
#include "process.hpp"

namespace wincpp::memory
{
    region_t::region_t( process_t *process, MEMORY_BASIC_INFORMATION mbi ) : process( process ), mbi( mbi )
    {
    }

    std::uintptr_t region_t::address() const noexcept
    {
        return reinterpret_cast< std::uintptr_t >( mbi.BaseAddress );
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

    std::size_t region_t::size() const noexcept
    {
        return static_cast< std::size_t >( mbi.RegionSize );
    }

    inline bool region_t::contains( std::uintptr_t address ) const noexcept
    {
        return address >= this->address() && address < this->address() + size();
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

    region_list::iterator::iterator( process_t *process, std::uintptr_t address ) : process( process ), address( address )
    {
        MEMORY_BASIC_INFORMATION info{};

        if ( address != -1 )
        {
            if ( !VirtualQueryEx( process->handle->native, reinterpret_cast< void * >( address ), &info, sizeof( info ) ) )
                throw core::error::from_win32( GetLastError() );

            this->address = reinterpret_cast< std::uintptr_t >( info.BaseAddress ) + info.RegionSize;
        }

        mbi = info;
    }

    region_list::iterator &region_list::iterator::operator++() noexcept
    {
        if ( VirtualQueryEx( process->handle->native, reinterpret_cast< void * >( address ), &mbi, sizeof( mbi ) ) == 0 )
            address = -1;
        else
            address += mbi.RegionSize;

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

    inline std::shared_ptr< std::uint8_t[] > region_t::read( std::uintptr_t address, std::size_t size ) const
    {
        return process->memory_factory.read( address, size );
    }

    inline std::size_t region_t::write( std::uintptr_t address, std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) const
    {
        return process->memory_factory.write( address, buffer, size );
    }

}  // namespace wincpp::memory