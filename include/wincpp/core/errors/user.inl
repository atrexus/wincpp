#pragma once

#include "wincpp/core/errors/user.hpp"

namespace wincpp::core
{
    inline const user_error_category& user_error_category::get() noexcept
    {
        static user_error_category instance;
        return instance;
    }

    inline const char* user_error_category::name() const noexcept
    {
        return "user";
    }

    inline std::string user_error_category::message( const int code ) const
    {
        switch ( static_cast< user_error_type_t >( code ) )
        {
            case user_error_type_t::module_not_found_t: return "The desired module was not found.";
            case user_error_type_t::thread_not_found_t: return "The desired thread was not found.";
            case user_error_type_t::process_not_found_t: return "The desired process was not found.";
            case user_error_type_t::export_not_found_t: return "The desired export was not found.";
            case user_error_type_t::memory_out_of_range_t: return "The requested memory range was outside the memory object.";
            case user_error_type_t::invalid_pattern_t: return "The requested pattern string could not be parsed.";
            case user_error_type_t::operation_failed_t: return "The requested operation failed.";
            default: return "Unknown error";
        }
    }
}  // namespace wincpp::core
