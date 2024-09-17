#include "patterns/scanner.hpp"

namespace wincpp::patterns
{
    scanner::scanner( std::shared_ptr< std::uint8_t[] > buffer, std::size_t size ) noexcept : buffer( buffer ), size( size )
    {
    }

    template<>
    static std::int64_t
    scanner::index_of< scanner::algorithm_t::naive_t >( const pattern_t& pattern, std::uint8_t* buffer, std::size_t size ) noexcept
    {
        const auto begin = buffer;

        for ( auto it = begin; it != begin + size; ++it )
        {
            for ( auto i = 0; i < pattern.size; ++i )
            {
                if ( pattern.mask[ i ] && pattern.bytes[ i ] != it[ i ] )
                    break;

                if ( i == pattern.size - 1 )
                    return static_cast< std::int64_t >( it - begin );
            }
        }

        return -1;
    }

    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::kmp_t >( const pattern_t& pattern, std::uint8_t* buffer, std::size_t size ) noexcept
    {
        // TODO: Implement
        return -1;
    }

    template<>
    static std::int64_t scanner::index_of< scanner::algorithm_t::bmh_t >( const pattern_t& pattern, std::uint8_t* buffer, std::size_t size ) noexcept
    {
        // TODO: Implement
        return -1;
    }

    template< scanner::algorithm_t algorithm >
    std::optional< std::uintptr_t > scanner::find( const pattern_t& pattern ) const noexcept
    {
        const auto result = scanner::index_of< algorithm >( pattern, buffer.get(), size );

        if ( result == -1 )
            return std::nullopt;

        return static_cast< std::uintptr_t >( result );
    }

    template< scanner::algorithm_t algorithm >
    std::vector< std::uintptr_t > scanner::find_all( const pattern_t& pattern ) const noexcept
    {
        std::vector< std::uintptr_t > results;
        const auto begin = buffer.get();

        for ( auto it = begin; it != begin + size; ++it )
        {
            const auto result = scanner::index_of< algorithm >( pattern, it, size - ( it - begin ) );

            if ( result != -1 )
                results.push_back( static_cast< std::uintptr_t >( result ) );
        }

        return results;
    }
}  // namespace wincpp::patterns