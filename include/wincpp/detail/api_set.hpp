#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <string>
#include <string_view>

#include "wincpp/detail/win32.hpp"

namespace wincpp::detail
{
    struct api_set_namespace_t
    {
        std::uint32_t version;
        std::uint32_t size;
        std::uint32_t flags;
        std::uint32_t count;
        std::uint32_t entry_offset;
        std::uint32_t hash_offset;
        std::uint32_t hash_factor;
    };

    struct api_set_namespace_entry_t
    {
        std::uint32_t flags;
        std::uint32_t name_offset;
        std::uint32_t name_length;
        std::uint32_t hashed_length;
        std::uint32_t value_offset;
        std::uint32_t value_count;
    };

    struct api_set_value_entry_t
    {
        std::uint32_t flags;
        std::uint32_t name_offset;
        std::uint32_t name_length;
        std::uint32_t value_offset;
        std::uint32_t value_length;
    };

    inline constexpr char ascii_lower( const char character ) noexcept
    {
        return character >= 'A' && character <= 'Z' ? static_cast< char >( character + ( 'a' - 'A' ) ) : character;
    }

    inline std::string ascii_lowercase( const std::string_view value )
    {
        std::string result( value );
        std::ranges::transform( result, result.begin(), ascii_lower );
        return result;
    }

    inline bool ascii_iequals( const std::string_view left, const std::string_view right ) noexcept
    {
        return left.size() == right.size() &&
               std::ranges::equal( left, right, []( const char a, const char b ) { return ascii_lower( a ) == ascii_lower( b ); } );
    }

    template< typename T >
    inline std::optional< T > read_api_set_value( const std::span< const std::uint8_t > schema, const std::size_t offset ) noexcept
    {
        if ( offset > schema.size() || sizeof( T ) > schema.size() - offset )
            return std::nullopt;

        T result{};
        std::memcpy( &result, schema.data() + offset, sizeof( result ) );
        return result;
    }

    inline std::optional< std::string >
    read_api_set_string( const std::span< const std::uint8_t > schema, const std::uint32_t offset, const std::uint32_t byte_length )
    {
        if ( byte_length % sizeof( wchar_t ) != 0 || offset > schema.size() || byte_length > schema.size() - offset )
            return std::nullopt;

        std::string result;
        result.reserve( byte_length / sizeof( wchar_t ) );
        for ( std::size_t index = 0; index < byte_length; index += sizeof( wchar_t ) )
        {
            wchar_t character{};
            std::memcpy( &character, schema.data() + offset + index, sizeof( character ) );
            if ( character > 0x7f )
                return std::nullopt;
            result.push_back( ascii_lower( static_cast< char >( character ) ) );
        }
        return result;
    }

    template< typename Memory >
    inline std::string
    api_set_host( const Memory& memory, const HANDLE process_handle, const std::string_view contract, const std::string_view importing_module = {} )
    {
        auto normalized_contract = ascii_lowercase( contract );
        if ( normalized_contract.ends_with( ".dll" ) )
            normalized_contract.resize( normalized_contract.size() - 4 );

        if ( !( normalized_contract.starts_with( "api-" ) || normalized_contract.starts_with( "ext-" ) ) )
            return {};

        struct process_basic_information_t
        {
            void* reserved1;
            void* peb_base_address;
            void* reserved2[ 2 ];
            std::uintptr_t unique_process_id;
            void* reserved3;
        } information{};

        using nt_query_information_process_t = LONG( NTAPI* )( HANDLE, ULONG, void*, ULONG, ULONG* );
        const auto query =
            reinterpret_cast< nt_query_information_process_t >( GetProcAddress( GetModuleHandleW( L"ntdll.dll" ), "NtQueryInformationProcess" ) );
        if ( !query || query( process_handle, 0, &information, sizeof( information ), nullptr ) < 0 || !information.peb_base_address )
            return {};

        constexpr auto api_set_map_offset = sizeof( void* ) == 8 ? std::uintptr_t{ 0x68 } : std::uintptr_t{ 0x38 };
        const auto peb = reinterpret_cast< std::uintptr_t >( information.peb_base_address );
        const auto api_set_map = memory.template read< std::uintptr_t >( peb + api_set_map_offset );
        if ( !api_set_map )
            return {};

        const auto header = memory.template read< api_set_namespace_t >( api_set_map );
        constexpr std::uint32_t maximum_schema_size = 16 * 1024 * 1024;
        if ( header.version < 6 || header.size < sizeof( header ) || header.size > maximum_schema_size )
            return {};

        const auto buffer = memory.read( api_set_map, header.size );
        if ( !buffer )
            return {};

        const auto schema = std::span< const std::uint8_t >( buffer.get(), header.size );
        if ( header.entry_offset > schema.size() || header.count > ( schema.size() - header.entry_offset ) / sizeof( api_set_namespace_entry_t ) )
            return {};

        std::string first_host;
        std::string default_host;

        for ( std::uint32_t index = 0; index < header.count; ++index )
        {
            const auto entry_offset = static_cast< std::size_t >( header.entry_offset ) + sizeof( api_set_namespace_entry_t ) * index;
            const auto entry = read_api_set_value< api_set_namespace_entry_t >( schema, entry_offset );
            if ( !entry )
                return {};

            const auto entry_name = read_api_set_string( schema, entry->name_offset, entry->name_length );
            if ( !entry_name )
                return {};
            if ( *entry_name != normalized_contract )
                continue;

            if ( entry->value_offset > schema.size() ||
                 entry->value_count > ( schema.size() - entry->value_offset ) / sizeof( api_set_value_entry_t ) )
                return {};

            for ( std::uint32_t value_index = 0; value_index < entry->value_count; ++value_index )
            {
                const auto value_offset = static_cast< std::size_t >( entry->value_offset ) + sizeof( api_set_value_entry_t ) * value_index;
                const auto value = read_api_set_value< api_set_value_entry_t >( schema, value_offset );
                if ( !value )
                    return {};

                const auto host = read_api_set_string( schema, value->value_offset, value->value_length );
                if ( !host )
                    return {};
                if ( host->empty() )
                    continue;
                if ( first_host.empty() )
                    first_host = *host;

                const auto alias = read_api_set_string( schema, value->name_offset, value->name_length );
                if ( !alias )
                    return {};
                if ( alias->empty() )
                    default_host = *host;
                else if ( !importing_module.empty() && ascii_iequals( *alias, importing_module ) )
                    return *host;
            }

            return default_host.empty() ? first_host : default_host;
        }

        return {};
    }
}  // namespace wincpp::detail
