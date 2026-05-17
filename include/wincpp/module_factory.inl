#pragma once

#include <exception>

#include "wincpp/module_factory.hpp"
#include "wincpp/modules/module.hpp"
#include "wincpp/process.hpp"

namespace wincpp
{
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
        for ( const auto& module : modules() )
        {
            if ( module->name() == name )
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
