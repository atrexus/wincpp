#pragma once

#include <concepts>
#include <cstdint>
#include <ostream>

#include "wincpp/memory/memory.hpp"

namespace wincpp::memory
{
    /// <summary>
    /// A class representing a pointer in the memory of the remote process.
    /// </summary>
    template< typename T >
    struct pointer_t : public memory_t
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
            /// <param name="factory">The process's memory factory.</param>
            explicit value_t( std::uintptr_t address, const memory_factory& factory ) noexcept;

            /// <summary>
            /// Converts the value to the type T.
            /// </summary>
            /// <returns>The value as type T.</returns>
            operator T() const;

            /// <summary>
            /// Assigns a value to the memory.
            /// </summary>
            /// <param name="value">The value to assign.</param>
            /// <returns>The value object.</returns>
            const value_t& operator=( const T& value ) const;

            /// <summary>
            /// Gets the values as a pointer.
            /// </summary>
            /// <returns>The pointer object.</returns>
            operator pointer_t< U >() const;

            /// <summary>
            /// Writes the value of the pointer to the output stream.
            /// </summary>
            /// <param name="os">The output stream.</param>
            /// <param name="value">The value object.</param>
            /// <returns>The output stream.</returns>
            friend std::ostream& operator<<( std::ostream& os, const value_t& value )
            {
                os << static_cast< T >( value );
                return os;
            }

            /// <summary>
            /// Compares the value with another value.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are equal.</returns>
            friend bool operator==( const value_t& lhs, const T& rhs )
            {
                return static_cast< T >( lhs ) == rhs;
            }

            /// <summary>
            /// Compares the value with another value.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are equal.</returns>
            friend bool operator==( const T& lhs, const value_t& rhs )
            {
                return lhs == static_cast< T >( rhs );
            }

            /// <summary>
            /// Compares the value with another value.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are not equal.</returns>
            friend bool operator!=( const value_t& lhs, const T& rhs )
            {
                return static_cast< T >( lhs ) != rhs;
            }

            /// <summary>
            /// Compares the value with another value.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are not equal.</returns>
            friend bool operator!=( const T& lhs, const value_t& rhs )
            {
                return lhs != static_cast< T >( rhs );
            }

            /// <summary>
            /// Compares two value objects.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are equal.</returns>
            friend bool operator==( const value_t& lhs, const value_t& rhs )
            {
                return static_cast< T >( lhs ) == static_cast< T >( rhs );
            }

            /// <summary>
            /// Compares two value objects.
            /// </summary>
            /// <param name="lhs">The left-hand side value.</param>
            /// <param name="rhs">The right-hand side value.</param>
            /// <returns>True if the values are not equal.</returns>
            friend bool operator!=( const value_t& lhs, const value_t& rhs )
            {
                return static_cast< T >( lhs ) != static_cast< T >( rhs );
            }

            std::uintptr_t address;
            const memory_factory* factory;
        };

        /// <summary>
        /// Creates a new pointer object.
        /// </summary>
        /// <param name="address">The address of the pointer.</param>
        /// <param name="factory">The process's memory factory.</param>
        explicit pointer_t( std::uintptr_t address, const memory_factory& factory ) noexcept;

        /// <summary>
        /// Converts this pointer to another pointer type.
        /// </summary>
        /// <typeparam name="U">The new value type.</typeparam>
        /// <returns>The converted pointer.</returns>
        template< typename U >
        operator pointer_t< U >() const;

        /// <summary>
        /// Dereferences the pointer.
        /// </summary>
        /// <returns>The value at the pointer.</returns>
        value_t< T > operator*() const noexcept;

        /// <summary>
        /// Converts the pointer to the address.
        /// </summary>
        /// <returns>The address of the pointer.</returns>
        operator std::uintptr_t() const noexcept;

        /// <summary>
        /// Returns whether the pointer is valid.
        /// </summary>
        /// <returns>True if the pointer points into a known memory region.</returns>
        explicit operator bool() const noexcept;

        /// <summary>
        /// Increments the pointer by the specified offset.
        /// </summary>
        /// <typeparam name="U">The offset type.</typeparam>
        /// <param name="offset">The offset to increment the pointer by.</param>
        /// <returns>The incremented pointer.</returns>
        template< std::integral U >
        pointer_t< T > operator+( U offset ) const noexcept;

        /// <summary>
        /// Writes the value of the pointer to the output stream.
        /// </summary>
        /// <param name="os">The output stream.</param>
        /// <param name="pointer">The pointer object.</param>
        /// <returns>The output stream.</returns>
        friend std::ostream& operator<<( std::ostream& os, const pointer_t& pointer )
        {
            os << "0x" << std::hex << pointer.value.address;
            return os;
        }

        /// <summary>
        /// Compares the pointer with another pointer.
        /// </summary>
        /// <param name="lhs">The left-hand side pointer.</param>
        /// <param name="rhs">The right-hand side pointer.</param>
        /// <returns>True if the pointers are equal.</returns>
        friend bool operator==( const pointer_t& lhs, const pointer_t& rhs )
        {
            return lhs.value.address == rhs.value.address;
        }

        /// <summary>
        /// Compares the pointer with another pointer.
        /// </summary>
        /// <param name="lhs">The left-hand side pointer.</param>
        /// <param name="rhs">The right-hand side pointer.</param>
        /// <returns>True if the pointers are not equal.</returns>
        friend bool operator!=( const pointer_t& lhs, const pointer_t& rhs )
        {
            return lhs.value.address != rhs.value.address;
        }

        value_t< T > value;
    };
}  // namespace wincpp::memory

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/memory/pointer.inl"
#endif
