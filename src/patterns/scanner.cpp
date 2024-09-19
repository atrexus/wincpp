#include "patterns/scanner.hpp"

#include "memory/memory.hpp"

namespace wincpp::patterns
{
    scanner::scanner( const memory::memory_t& object ) : object( object )
    {
    }

    template<>
    static std::int64_t
    scanner::index_of< scanner::algorithm_t::naive_t >( const pattern_t& pattern, std::uint8_t* buffer, std::size_t size ) noexcept
    {
        const auto begin = buffer;

        for ( auto it = begin; it != ( begin + size ); ++it )
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
    static std::int64_t
    scanner::index_of< scanner::algorithm_t::bmh_t >( const pattern_t& pattern, std::uint8_t* buffer, std::size_t buffer_size ) noexcept
    {
        if ( pattern.size == 0 || buffer_size == 0 || pattern.size > buffer_size )
        {
            return -1;
        }

        // Build the skip table for the Boyer-Moore-Horspool algorithm
        std::size_t skip_table[ 256 ];  // Assume byte values range from 0 to 255

        std::fill( std::begin( skip_table ), std::end( skip_table ), pattern.size );

        for ( std::size_t i = 0; i < pattern.size - 1; ++i )
        {
            if ( pattern.mask[ i ] )
            {
                // Only build the table for strict bytes
                skip_table[ pattern.bytes[ i ] ] = pattern.size - 1 - i;
            }
        }

        // Perform the search
        std::int64_t buffer_idx = 0;

        while ( buffer_idx <= static_cast< std::int64_t >( buffer_size - pattern.size ) )
        {
            std::int64_t pattern_idx = pattern.size - 1;

            // Match from the end of the pattern
            while ( pattern_idx >= 0 )
            {
                if ( pattern.mask[ pattern_idx ] && pattern.bytes[ pattern_idx ] != buffer[ buffer_idx + pattern_idx ] )
                {
                    break;  // Strict match failed, exit loop
                }
                --pattern_idx;
            }

            if ( pattern_idx < 0 )
            {
                return buffer_idx;  // Pattern found
            }

            // Use the skip table to jump forward
            std::uint8_t last_byte = buffer[ buffer_idx + pattern.size - 1 ];
            buffer_idx += skip_table[ last_byte ];
        }

        return -1;  // No match found
    }

    template<>
    static std::int64_t
    scanner::index_of< scanner::algorithm_t::raita_t >( const pattern_t& pattern, std::uint8_t* buffer, std::size_t buffer_size ) noexcept
    {
        if ( pattern.size == 0 || buffer_size == 0 || pattern.size > buffer_size )
        {
            return -1;
        }

        // Raita uses a combination of the first, middle, and last bytes for an efficient search
        std::size_t last_idx = pattern.size - 1;
        std::size_t mid_idx = pattern.size / 2;

        // Build the skip table for the Raita algorithm
        std::size_t skip_table[ 256 ];
        std::fill( std::begin( skip_table ), std::end( skip_table ), pattern.size );

        for ( std::size_t i = 0; i < last_idx; ++i )
        {
            if ( pattern.mask[ i ] )
            {
                skip_table[ pattern.bytes[ i ] ] = last_idx - i;
            }
        }

        std::int64_t buffer_idx = 0;

        while ( buffer_idx <= static_cast< std::int64_t >( buffer_size - pattern.size ) )
        {
            // Check the last byte first
            if ( !pattern.mask[ last_idx ] || pattern.bytes[ last_idx ] == buffer[ buffer_idx + last_idx ] )
            {
                // Check the first byte
                if ( !pattern.mask[ 0 ] || pattern.bytes[ 0 ] == buffer[ buffer_idx ] )
                {
                    // Check the middle byte
                    if ( !pattern.mask[ mid_idx ] || pattern.bytes[ mid_idx ] == buffer[ buffer_idx + mid_idx ] )
                    {
                        // Now verify the rest of the pattern
                        std::size_t pattern_idx = 1;
                        while ( pattern_idx < last_idx &&
                                ( !pattern.mask[ pattern_idx ] || pattern.bytes[ pattern_idx ] == buffer[ buffer_idx + pattern_idx ] ) )
                        {
                            ++pattern_idx;
                        }

                        if ( pattern_idx == last_idx )
                        {
                            return buffer_idx;  // Full pattern match
                        }
                    }
                }
            }

            // Skip using the skip table
            std::uint8_t last_byte = buffer[ buffer_idx + last_idx ];
            buffer_idx += skip_table[ last_byte ];
        }

        return -1;  // No match found
    }

}  // namespace wincpp::patterns