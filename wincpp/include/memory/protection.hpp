#pragma once

#include <Windows.h>

#include <bitset>
#include <limits>
#include <ostream>

namespace wincpp::memory
{
    /// <summary>
    /// Specifies memory protection constants. These values are used to set the memory protection
    /// options for a range of memory pages in a process's virtual address space.
    /// </summary>
    enum class protection_t : std::uint32_t
    {
        /// <summary>
        /// Disables all access to the committed region of pages. Any attempt to read, write, or execute
        /// the committed region results in an access violation.
        /// </summary>
        noaccess_t = PAGE_NOACCESS,

        /// <summary>
        /// Enables read-only access to the committed region of pages. Any attempt to write to the
        /// committed region results in an access violation. If Data Execution Prevention (DEP) is enabled,
        /// attempts to execute code in the committed region result in an access violation.
        /// </summary>
        readonly_t = PAGE_READONLY,

        /// <summary>
        /// Enables read and write access to the committed region of pages. If DEP is enabled, attempts
        /// to execute code in the committed region result in an access violation.
        /// </summary>
        readwrite_t = PAGE_READWRITE,

        /// <summary>
        /// Enables copy-on-write access to the committed region of pages. When a process writes to the committed region,
        /// the system copies the original pages to a new region, and the process receives access to the new region.
        /// </summary>
        writecopy_t = PAGE_WRITECOPY,

        /// <summary>
        /// Enables execute-only access to the committed region of pages. Any attempt to read or write to
        /// the committed region results in an access violation.
        /// </summary>
        execute_t = PAGE_EXECUTE,

        /// <summary>
        /// Enables execute and read access to the committed region of pages. Any attempt to write to the
        /// committed region results in an access violation.
        /// </summary>
        execute_read_t = PAGE_EXECUTE_READ,

        /// <summary>
        /// Enables execute, read, and write access to the committed region of pages.
        /// </summary>
        execute_readwrite_t = PAGE_EXECUTE_READWRITE,

        /// <summary>
        /// Enables execute and copy-on-write access to the committed region of pages.
        /// </summary>
        execute_writecopy_t = PAGE_EXECUTE_WRITECOPY,

        /// <summary>
        /// Marks the committed region of pages as guarded. Any attempt to access a guarded region
        /// of pages causes the system to raise a guard violation exception.
        /// </summary>
        guard_t = PAGE_GUARD,

        /// <summary>
        /// Disables caching for the committed region of pages.
        /// </summary>
        nocache_t = PAGE_NOCACHE,

        /// <summary>
        /// Enables write-combining optimization for the committed region of pages. This allows for faster
        /// writes to certain types of memory (such as video memory), but can result in less predictable
        /// memory ordering.
        /// </summary>
        writecombine_t = PAGE_WRITECOMBINE,

        /// <summary>
        /// Marks the committed region of pages as invalid for control-flow enforcement technology (CET).
        /// Any attempt to use this memory as a target for indirect control flow (such as function pointers)
        /// will result in an exception.
        /// </summary>
        targets_invalid_t = PAGE_TARGETS_INVALID,

        /// <summary>
        /// Marks the committed region of pages as non-updateable for CET. This prevents further updates
        /// to the control flow protection settings of the pages.
        /// </summary>
        targets_no_update_t = PAGE_TARGETS_NO_UPDATE,
    };

    struct protection_flags_t final
    {
        static const protection_flags_t execute_readwrite;

        /// <summary>
        /// Creates a new protection flags object.
        /// </summary>
        /// <param name="flags">The flags to set.</param>
        protection_flags_t( std::uint32_t flags ) noexcept;

        /// <summary>
        /// Creates a new protection flags object.
        /// </summary>
        /// <param name="flags">The flags to set.</param>
        template< typename... Flags, typename = typename std::enable_if_t< ( std::is_same_v< Flags, protection_t > && ... ) > >
        explicit protection_flags_t( Flags... flags ) noexcept : flags( 0 )
        {
            ( add( flags ), ... );
        }

        /// <summary>
        /// Adds a flag to the protection flags.
        /// </summary>
        inline void add( protection_t protection ) noexcept;

        /// <summary>
        /// Removes a flag from the protection flags.
        /// </summary>
        inline void remove( protection_t protection ) noexcept;

        /// <summary>
        /// Checks if the protection flags contain a specific flag.
        /// </summary>
        inline bool has( protection_t protection ) const noexcept;

        /// <summary>
        /// Gets the protection flags.
        /// </summary>
        std::uint32_t get() const noexcept;

        /// <summary>
        /// Checks if the protection flags are equal.
        /// </summary>
        /// <param name="lhs">The left-hand side protection flags.</param>
        /// <param name="rhs">The right-hand side protection flags.</param>
        /// <returns>True if the protection flags are equal, false otherwise.</returns>
        friend bool operator==( const protection_flags_t& lhs, const protection_flags_t& rhs );

        /// <summary>
        /// Writes the protection flags to the output stream.
        /// </summary>
        /// <param name="os">The output stream.</param>
        /// <param name="flags">The protection flags object.</param>
        friend std::ostream& operator<<( std::ostream& os, const protection_flags_t& flags );

       private:
        std::uint32_t flags;
    };

}  // namespace wincpp::memory