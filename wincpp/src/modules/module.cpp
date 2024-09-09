#include "modules/module.hpp"

#include "process.hpp"

#include <algorithm>

namespace wincpp::modules
{
    module_t::module_t( process_t *process, const core::module_entry_t &entry ) noexcept : process( process ), entry( entry ), info()
    {
        GetModuleInformation( process->handle->native, reinterpret_cast< HMODULE >( entry.base_address ), &info, sizeof( info ) );

        // Load the module data into the buffer.
        buffer = process->memory_factory.read( entry.base_address, entry.base_size );

        // Get the DOS header.
        dos_header = reinterpret_cast< const IMAGE_DOS_HEADER * >( buffer.get() );

        // Get the NT headers.
        nt_headers = reinterpret_cast< const IMAGE_NT_HEADERS * >( buffer.get() + dos_header->e_lfanew );
    }

    std::string module_t::name() const noexcept
    {
        std::string name( entry.name );

        // Convert the name to lowercase.
        std::transform( name.begin(), name.end(), name.begin(), ::tolower );

        return name;
    }

    std::uintptr_t module_t::base() const noexcept
    {
        return entry.base_address;
    }

    std::uintptr_t module_t::entry_point() const noexcept
    {
        return reinterpret_cast< std::uintptr_t >( info.EntryPoint );
    }

    std::string module_t::path() const noexcept
    {
        return entry.path;
    }

    std::size_t module_t::size() const noexcept
    {
        return static_cast< std::size_t >( entry.base_size );
    }

    std::optional< module_t::export_t > module_t::fetch_export( const std::string_view name ) const
    {
        const auto directory_header = nt_headers->OptionalHeader.DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ];

        if ( !directory_header.VirtualAddress )
            return std::nullopt;

        const auto export_directory = reinterpret_cast< const IMAGE_EXPORT_DIRECTORY * >( buffer.get() + directory_header.VirtualAddress );

        const auto names = reinterpret_cast< const std::uint32_t * >( buffer.get() + export_directory->AddressOfNames );
        const auto ordinals = reinterpret_cast< const std::uint16_t * >( buffer.get() + export_directory->AddressOfNameOrdinals );
        const auto functions = reinterpret_cast< const std::uint32_t * >( buffer.get() + export_directory->AddressOfFunctions );

        for ( std::uint32_t i = 0; i < export_directory->NumberOfNames; ++i )
        {
            const auto export_name = reinterpret_cast< const char * >( buffer.get() + names[ i ] );

            if ( name == export_name )
            {
                const auto ordinal = ordinals[ i ];
                const auto address = functions[ ordinal ];

                return export_t( *this, export_name, address, ordinal );
            }
        }

        return std::nullopt;
    }

    module_t::export_t module_t::operator[]( const std::string_view name ) const
    {
        return *fetch_export( name );
    }

    module_list_t::module_list_t( process_t *process ) noexcept
        : process( process ),
          snapshot( core::snapshot< core::snapshot_kind::module_t >::create( process->id ) )
    {
    }

    module_list_t::iterator module_list_t::begin() const noexcept
    {
        return iterator( process, snapshot.begin() );
    }

    module_list_t::iterator module_list_t::end() const noexcept
    {
        return iterator( process, snapshot.end() );
    }

    module_list_t::iterator::iterator( process_t *process, const core::snapshot< core::snapshot_kind::module_t >::iterator &it ) noexcept
        : process( process ),
          it( it )
    {
    }

    module_t module_list_t::iterator::operator*() const noexcept
    {
        return module_t( process, *it );
    }

    module_list_t::iterator &module_list_t::iterator::operator++()
    {
        ++it;
        return *this;
    }

    bool module_list_t::iterator::operator==( const iterator &other ) const noexcept
    {
        return it == other.it;
    }

    bool module_list_t::iterator::operator!=( const iterator &other ) const noexcept
    {
        return !operator==( other );
    }
}  // namespace wincpp::modules