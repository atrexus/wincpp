#pragma once

#include <array>
#include <string_view>
#include <utility>

#include "wincpp/memory/protection.hpp"

namespace wincpp::memory
{
    inline const protection_flags_t protection_flags_t::execute_readwrite{ protection_t::execute_readwrite_t };
    inline const protection_flags_t protection_flags_t::readwrite{ protection_t::readwrite_t };
    inline const protection_flags_t protection_flags_t::noaccess{ protection_t::noaccess_t };
    inline const protection_flags_t protection_flags_t::guard{ protection_t::guard_t };

    inline protection_flags_t::protection_flags_t( const std::uint32_t flags ) noexcept : flags( flags )
    {
    }

    template< typename... Flags >
        requires( std::same_as< Flags, protection_t > && ... )
    protection_flags_t::protection_flags_t( Flags... flags ) noexcept : flags( 0 )
    {
        ( add( flags ), ... );
    }

    inline void protection_flags_t::add( const protection_t protection ) noexcept
    {
        flags |= std::to_underlying( protection );
    }

    inline void protection_flags_t::remove( const protection_t protection ) noexcept
    {
        flags &= ~std::to_underlying( protection );
    }

    inline bool protection_flags_t::has( const protection_t protection ) const noexcept
    {
        return ( flags & std::to_underlying( protection ) ) != 0;
    }

    inline std::uint32_t protection_flags_t::get() const noexcept
    {
        return flags;
    }

    inline bool operator==( const protection_flags_t& lhs, const protection_flags_t& rhs ) noexcept
    {
        return lhs.flags == rhs.flags;
    }

    inline std::ostream& operator<<( std::ostream& os, const protection_flags_t& flags )
    {
        static constexpr std::array values{
            std::pair{ protection_t::noaccess_t, std::string_view{ "noaccess" } },
            std::pair{ protection_t::readonly_t, std::string_view{ "readonly" } },
            std::pair{ protection_t::readwrite_t, std::string_view{ "readwrite" } },
            std::pair{ protection_t::writecopy_t, std::string_view{ "writecopy" } },
            std::pair{ protection_t::execute_t, std::string_view{ "execute" } },
            std::pair{ protection_t::execute_read_t, std::string_view{ "execute_read" } },
            std::pair{ protection_t::execute_readwrite_t, std::string_view{ "execute_readwrite" } },
            std::pair{ protection_t::execute_writecopy_t, std::string_view{ "execute_writecopy" } },
            std::pair{ protection_t::guard_t, std::string_view{ "guard" } },
            std::pair{ protection_t::nocache_t, std::string_view{ "nocache" } },
            std::pair{ protection_t::writecombine_t, std::string_view{ "writecombine" } },
            std::pair{ protection_t::targets_invalid_t, std::string_view{ "targets_invalid" } },
        };

        bool first = true;

        for ( const auto& [ value, name ] : values )
        {
            if ( flags.has( value ) )
            {
                if ( !first )
                    os << " | ";

                os << name;
                first = false;
            }
        }

        if ( first )
            os << "none";

        return os;
    }
}  // namespace wincpp::memory
