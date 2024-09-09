#include "process.hpp"

#include <algorithm>

namespace wincpp
{
    module_factory::module_factory( process_t* p ) noexcept : p( p )
    {
    }

    modules::module_list_t module_factory::modules() const
    {
        return modules::module_list_t( p );
    }

    modules::module_t wincpp::module_factory::main_module() const
    {
        for ( const auto& module : modules() )
        {
            if ( module.name() == p->name )
            {
                return module;
            }
        }

        throw std::runtime_error( "Failed to find the main module." );
    }

    modules::module_t module_factory::fetch_module( const std::string_view name ) const
    {
        std::string data( name );

        // Convert the string to lowercase.
        std::transform( data.begin(), data.end(), data.begin(), ::tolower );

        // If the data doesn't contain the extension, add it.
        if ( data.find( ".dll" ) == std::string::npos )
        {
            data.append( ".dll" );
        }

        for ( const auto& module : modules() )
        {
            if ( module.name() == data )
            {
                return module;
            }
        }

        throw std::runtime_error( "Failed to find the module." );
    }

    modules::module_t module_factory::operator[]( const std::string_view name ) const
    {
        return fetch_module( name );
    }
}  // namespace wincpp