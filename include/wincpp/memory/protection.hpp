#pragma once

#include <concepts>
#include <cstdint>
#include <ostream>

namespace wincpp::memory
{
    /// <summary>
    /// Specifies memory protection constants.
    /// </summary>
    enum class protection_t : std::uint32_t
    {
        /// <summary>
        /// Disables all access to the committed region of pages.
        /// </summary>
        noaccess_t = 0x00000001,

        /// <summary>
        /// Enables read-only access to the committed region of pages.
        /// </summary>
        readonly_t = 0x00000002,

        /// <summary>
        /// Enables read and write access to the committed region of pages.
        /// </summary>
        readwrite_t = 0x00000004,

        /// <summary>
        /// Enables copy-on-write access to the committed region.
        /// </summary>
        writecopy_t = 0x00000008,

        /// <summary>
        /// Enables execute-only access to the committed region.
        /// </summary>
        execute_t = 0x00000010,

        /// <summary>
        /// Enables execute and read access to the committed region.
        /// </summary>
        execute_read_t = 0x00000020,

        /// <summary>
        /// Enables execute, read, and write access to the committed region.
        /// </summary>
        execute_readwrite_t = 0x00000040,

        /// <summary>
        /// Enables execute and copy-on-write access to the committed region.
        /// </summary>
        execute_writecopy_t = 0x00000080,

        /// <summary>
        /// Marks the committed region of pages as guarded.
        /// </summary>
        guard_t = 0x00000100,

        /// <summary>
        /// Disables caching for the committed region.
        /// </summary>
        nocache_t = 0x00000200,

        /// <summary>
        /// Enables write-combining optimization for the committed region.
        /// </summary>
        writecombine_t = 0x00000400,

        /// <summary>
        /// Marks the committed region of pages as invalid for control-flow enforcement technology.
        /// </summary>
        targets_invalid_t = 0x40000000,

        /// <summary>
        /// Marks the committed region of pages as non-updateable for control-flow enforcement technology.
        /// </summary>
        targets_no_update_t = 0x40000000
    };

    /// <summary>
    /// Contains a set of page protection flags.
    /// </summary>
    struct protection_flags_t final
    {
        /// <summary>
        /// Execute, read, and write access.
        /// </summary>
        static const protection_flags_t execute_readwrite;

        /// <summary>
        /// Read and write access.
        /// </summary>
        static const protection_flags_t readwrite;

        /// <summary>
        /// No access.
        /// </summary>
        static const protection_flags_t noaccess;

        /// <summary>
        /// Guarded access.
        /// </summary>
        static const protection_flags_t guard;

        /// <summary>
        /// Creates a new protection flags object.
        /// </summary>
        /// <param name="flags">The raw flags to set.</param>
        explicit protection_flags_t( std::uint32_t flags = 0 ) noexcept;

        /// <summary>
        /// Creates a new protection flags object.
        /// </summary>
        /// <typeparam name="Flags">The protection flag types.</typeparam>
        /// <param name="flags">The flags to set.</param>
        template< typename... Flags >
            requires( std::same_as< Flags, protection_t > && ... )
        explicit protection_flags_t( Flags... flags ) noexcept;

        /// <summary>
        /// Adds a flag to the protection flags.
        /// </summary>
        /// <param name="protection">The protection flag to add.</param>
        void add( protection_t protection ) noexcept;

        /// <summary>
        /// Removes a flag from the protection flags.
        /// </summary>
        /// <param name="protection">The protection flag to remove.</param>
        void remove( protection_t protection ) noexcept;

        /// <summary>
        /// Checks if the protection flags contain a specific flag.
        /// </summary>
        /// <param name="protection">The protection flag to check.</param>
        /// <returns>True if the flag is present.</returns>
        bool has( protection_t protection ) const noexcept;

        /// <summary>
        /// Gets the raw protection flags.
        /// </summary>
        /// <returns>The raw flags.</returns>
        std::uint32_t get() const noexcept;

        /// <summary>
        /// Checks if the protection flags are equal.
        /// </summary>
        /// <param name="lhs">The left-hand side protection flags.</param>
        /// <param name="rhs">The right-hand side protection flags.</param>
        /// <returns>True if the protection flags are equal, false otherwise.</returns>
        friend bool operator==( const protection_flags_t& lhs, const protection_flags_t& rhs ) noexcept;

        /// <summary>
        /// Writes the protection flags to the output stream.
        /// </summary>
        /// <param name="os">The output stream.</param>
        /// <param name="flags">The protection flags object.</param>
        /// <returns>The output stream.</returns>
        friend std::ostream& operator<<( std::ostream& os, const protection_flags_t& flags );

       private:
        std::uint32_t flags;
    };
}  // namespace wincpp::memory

#ifndef WINCPP_SUPPRESS_AUTO_INL
#include "wincpp/memory/protection.inl"
#endif
