#include "modules/module.hpp"

#include <algorithm>

#include "modules/object.hpp"
#include "modules/section.hpp"
#include "patterns/scanner.hpp"
#include "process.hpp"

namespace wincpp::modules
{
    module_t::module_t( const memory_factory &factory, const core::module_entry_t &entry ) noexcept
        : memory_t( factory, entry.base_address, entry.base_size ),
          entry( entry ),
          info()
    {
        GetModuleInformation( factory.p->handle->native, reinterpret_cast< HMODULE >( entry.base_address ), &info, sizeof( info ) );

        // Load the module data into the buffer. We read the first page of the module as that is the maximum size of the headers.
        buffer = factory.read( entry.base_address, 0x1000 );

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

    std::uintptr_t module_t::entry_point() const noexcept
    {
        return reinterpret_cast< std::uintptr_t >( info.EntryPoint );
    }

    std::string module_t::path() const noexcept
    {
        return entry.path;
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

    std::optional< module_t::section_t > module_t::fetch_section( const std::string_view name ) const
    {
        const auto section = IMAGE_FIRST_SECTION( nt_headers );

        for ( std::uint16_t i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i )
        {
            const auto current_section = section[ i ];

            if ( name == reinterpret_cast< const char * >( current_section.Name ) )
                return section_t( *this, current_section );
        }

        return std::nullopt;
    }

    std::vector< std::shared_ptr< module_t::object_t > > module_t::fetch_objects( const std::string_view mangled ) const
    {
        std::vector< std::shared_ptr< module_t::object_t > > objects;

        // Get the sections that we need for location.
        const auto &data = fetch_section( ".data" );
        const auto &rdata = fetch_section( ".rdata" );

        if ( !data || !rdata )
            return {};

        const auto result = data->scanner().find< patterns::scanner::algorithm_t::bmh_t >( patterns::pattern_t::from( mangled ) );

        if ( !result )
            return {};

        // Calculate the type descriptor address based on the match for the string.
        const auto type_descriptor_address = *result - sizeof( std::uintptr_t ) * 2;
        const auto type_descriptor_rva = static_cast< std::int32_t >( type_descriptor_address - address() );

        // Find all cross references to the type descriptor in the .rdata section.
        const auto cross_references =
            rdata->scanner().find_all< patterns::scanner::algorithm_t::bmh_t >( patterns::pattern_t::from( type_descriptor_rva ) );

        for ( const auto &reference : cross_references )
        {
            const auto col_address = reference - sizeof( std::uint32_t ) * 3;

            // Now we read the complete object locator and check if its valid.
            const auto col = read< rtti::complete_object_locator_t >( col_address );

            if ( col.signature != 1 )
                continue;

            // Now we know that we've located a valid object. Now we need to locate the vtable address associated with the current complete object
            // locator.
            const auto col_reference = rdata->scanner().find< patterns::scanner::algorithm_t::bmh_t >( patterns::pattern_t::from( col_address ) );

            if ( !col_reference )
                continue;

            objects.emplace_back( new module_t::object_t( *this, *col_reference + sizeof( std::uintptr_t ), col ) );
        }

        return objects;
    }

    module_t::export_t module_t::operator[]( const std::string_view name ) const
    {
        return *fetch_export( name );
    }

    module_list::module_list( process_t *process ) noexcept
        : process( process ),
          snapshot( core::snapshot< core::snapshot_kind::module_t >::create( process->id() ) )
    {
    }

    module_list::iterator module_list::begin() const noexcept
    {
        return iterator( process, snapshot.begin() );
    }

    module_list::iterator module_list::end() const noexcept
    {
        return iterator( process, snapshot.end() );
    }

    module_list::iterator::iterator( process_t *process, const core::snapshot< core::snapshot_kind::module_t >::iterator &it ) noexcept
        : process( process ),
          it( it )
    {
    }

    module_t module_list::iterator::operator*() const noexcept
    {
        return module_t( process->memory_factory, *it );
    }

    module_list::iterator &module_list::iterator::operator++()
    {
        ++it;
        return *this;
    }

    bool module_list::iterator::operator==( const iterator &other ) const noexcept
    {
        return it == other.it;
    }

    bool module_list::iterator::operator!=( const iterator &other ) const noexcept
    {
        return !operator==( other );
    }
}  // namespace wincpp::modules