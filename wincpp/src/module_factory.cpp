#include "process.hpp"

namespace wincpp
{
    module_factory::module_factory( process_t *p ) noexcept : p( p )
    {
    }

    modules::module_list_t module_factory::list() const
    {
        return modules::module_list_t();
    }
}  // namespace wincpp