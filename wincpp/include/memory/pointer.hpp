#pragma once

#include <iostream>

#include "process.hpp"

namespace wincpp::memory
{
    /// <summary>
    /// A class representing a pointer in the memory of the remote process.
    /// </summary>
    template< typename T >
    struct pointer_t final
    {
        /// <summary>
        /// A class representing a value in the memory of the remote process.
        /// </summary>
        template< typename U >
        struct value_t final
        {
            /// <summary>
            /// Creates a new value object.
            /// </summary>
            /// <param name="address">The address of the value.</param>
            /// <param name="process">The process object.</param>
            explicit value_t( std::uintptr_t address, process_t *process ) noexcept : address( address ), process( process )
            {
            }

            /// <summary>
            /// Converts the value to the type T.
            /// </summary>
            /// <returns>The value as type T.</returns>
            inline operator T() const
            {
                return process->memory_factory.read< T >( address );
            }

            /// <summary>
            /// Assigns a value to the memory.
            /// </summary>
            inline const value_t &operator=( T value ) const
            {
                process->memory_factory.write< T >( address, value );
                return *this;
            }

            /// <summary>
            /// Writes the value of the pointer to the output stream.
            /// </summary>
            /// <param name="os">The output stream.</param>
            /// <param name="value">The value object.</param>
            /// <returns>The output stream.</returns>
            friend std::ostream &operator<<( std::ostream &os, const value_t &value )
            {
                os << static_cast< T >( value );
                return os;
            }

            /// <summary>
            /// Compares the value with another value.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are equal, false otherwise.</returns>
            friend bool operator==( const value_t &lhs, const T &rhs )
            {
                return static_cast< T >( lhs ) == rhs;
            }

            /// <summary>
            /// Compares the value with another value.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are equal, false otherwise.</returns>
            friend bool operator==( const T &lhs, const value_t &rhs )
            {
                return lhs == static_cast< T >( rhs );
            }

            /// <summary>
            /// Compares the value with another value.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are not equal, false otherwise.</returns>
            friend bool operator!=( const value_t &lhs, const T &rhs )
            {
                return static_cast< T >( lhs ) != rhs;
            }

            /// <summary>
            /// Compares the value with another value.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are not equal, false otherwise.</returns>
            friend bool operator!=( const T &lhs, const value_t &rhs )
            {
                return lhs != static_cast< T >( rhs );
            }

            std::uintptr_t address;
            process_t *process;
        };

        /// <summary>
        /// Creates a new pointer object.
        /// </summary>
        /// <param name="address">The address of the pointer.</param>
        /// <param name="process">The process object.</param>
        explicit pointer_t( const std::uintptr_t address, process_t *process ) noexcept : value( address, process )
        {
        }

        template< typename U >
        inline operator pointer_t< U >() const
        {
            return pointer_t< U >( value.address, value.process );
        }

        /// <summary>
        /// Dereferences the pointer.
        /// </summary>
        /// <returns>The value at the pointer.</returns>
        inline value_t< T > operator*() const noexcept
        {
            return value;
        }

        /// <summary>
        /// Converts the pointer to the address.
        /// </summary>
        /// <returns>The address of the pointer.</returns>
        inline operator std::uintptr_t() const noexcept
        {
            return value.address;
        }

        /// <summary>
        /// Returns whether the pointer is valid.
        /// </summary>
        /// <remarks>
        /// Todo: Search for a memory region that contains the pointer. If the region is not found, the pointer is invalid.
        /// </remarks>
        inline operator bool() const noexcept
        {
            if ( value.address != 0 )
            {
                for ( const auto &region : value.process->memory_factory.regions() )
                {
                    if ( region.contains( value.address ) )
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        /// <summary>
        /// Writes the value of the pointer to the output stream.
        /// </summary>
        /// <param name="os">The output stream.</param>
        /// <param name="pointer">The pointer object.</param>
        /// <returns>The output stream.</returns>
        friend std::ostream &operator<<( std::ostream &os, const pointer_t &pointer )
        {
            os << "0x" << std::hex << pointer.value.address;
            return os;
        }

        /// <summary>
        /// Compares the pointer with another pointer.
        /// </summary>
        /// <param name="lhs">The left-hand side pointer.</param>
        /// <param name="rhs">The right-hand side pointer.</param>
        /// <returns>True if the pointers are equal, false otherwise.</returns>
        friend bool operator==( const pointer_t &lhs, const pointer_t &rhs )
        {
            return lhs.value.address == rhs.value.address;
        }

        /// <summary>
        /// Compares the pointer with another pointer.
        /// </summary>
        /// <param name="lhs">The left-hand side pointer.</param>
        /// <param name="rhs">The right-hand side pointer.</param>
        /// <returns>True if the pointers are not equal, false otherwise.</returns>
        friend bool operator!=( const pointer_t &lhs, const pointer_t &rhs )
        {
            return lhs.value.address != rhs.value.address;
        }

        value_t< T > value;
    };
}  // namespace wincpp::memory