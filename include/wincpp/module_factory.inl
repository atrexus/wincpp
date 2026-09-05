#pragma once

#include <exception>
#include <string>

#include "wincpp/detail/api_set.hpp"
#include "wincpp/module_factory.hpp"
#include "wincpp/modules/module.hpp"
#include "wincpp/process.hpp"

namespace wincpp
{
    namespace detail
    {
        inline std::string_view module_base_name( const std::string_view path ) noexcept
        {
            const auto separator = path.find_last_of( "\\/" );
            return separator == std::string_view::npos ? path : path.substr( separator + 1 );
        }

        inline bool module_name_has_extension( const std::string_view name ) noexcept
        {
            return name.find_last_of( '.' ) != std::string_view::npos;
        }
    }  // namespace detail

    inline module_factory::module_factory( process_t* const p ) noexcept : p( p )
    {
    }

    inline const std::vector< std::shared_ptr< modules::module_t > >& module_factory::modules() const
    {
        return modules( false );
    }

    inline const std::vector< std::shared_ptr< modules::module_t > >& module_factory::modules( const bool refresh_cache ) const
    {
        if ( refresh_cache )
            refresh();

        for ( const auto& entry : core::snapshot< core::snapshot_kind::module_t >::create( p->id() ) )
        {
            if ( module_keys.contains( entry.base_address ) )
                continue;

            module_keys.insert( entry.base_address );
            module_list.emplace_back( std::shared_ptr< modules::module_t >( new modules::module_t( p->memory_factory, entry ) ) );
        }

        return module_list;
    }

    inline void module_factory::refresh() const
    {
        module_list.clear();
        module_keys.clear();
    }

    inline const modules::module_t& module_factory::main_module() const
    {
        const auto& module_list = modules();

        if ( module_list.empty() )
            throw core::error::from_user( core::user_error_type_t::module_not_found_t, "Failed to find main module" );

        return *module_list.front();
    }

    inline std::shared_ptr< modules::module_t > module_factory::fetch_module( const std::string_view name ) const
    {
        return fetch_module( name, {} );
    }

    inline std::shared_ptr< modules::module_t > module_factory::fetch_module( const std::string_view name, const std::string_view importing_module )
        const
    {
        const auto requested_base_name = detail::module_base_name( name );
        const auto api_set_target =
            detail::api_set_host( p->memory_factory, static_cast< HANDLE >( p->handle->native ), requested_base_name, importing_module );
        const auto requested_name = api_set_target.empty() ? requested_base_name : std::string_view( api_set_target );
        const auto dll_name = detail::module_name_has_extension( requested_name ) ? std::string{} : std::string{ requested_name } + ".dll";

        for ( const auto& module : modules() )
        {
            if ( detail::ascii_iequals( module->name(), requested_name ) ||
                 ( !dll_name.empty() && detail::ascii_iequals( module->name(), dll_name ) ) )
                return module;
        }

        return nullptr;
    }

    inline core::result_t< std::shared_ptr< modules::module_t > > module_factory::try_fetch_module( const std::string_view name ) const noexcept
    {
        try
        {
            if ( auto result = fetch_module( name ) )
                return result;

            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::module_not_found_t, "Failed to find module \"{}\"", name ) };
        }
        catch ( const core::error& error )
        {
            return core::unexpected_t{ error };
        }
        catch ( const std::exception& exception )
        {
            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::operation_failed_t, "{}", exception.what() ) };
        }
        catch ( ... )
        {
            return core::unexpected_t{ core::error::from_user( core::user_error_type_t::operation_failed_t, "Unknown failure" ) };
        }
    }

    inline const modules::module_t& module_factory::operator[]( const std::string_view name ) const
    {
        if ( const auto result = fetch_module( name ) )
            return *result;

        throw core::error::from_user( core::user_error_type_t::module_not_found_t, "Failed to find module \"{}\"", name );
    }
}  // namespace wincpp
