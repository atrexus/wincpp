#include <system_error>

namespace wincpp::core
{
    /// <summary>
    /// The error category for Win32 errors.
    /// </summary>
    struct win32_error_category : public std::error_category
    {
        /// <summary>
        /// Returns the error category for Win32 errors.
        /// </summary>
        static const win32_error_category& get() noexcept;

        /// <summary>
        /// Returns the name of the error category.
        /// </summary>
        const char* name() const noexcept override;
        
        /// <summary>
        /// Returns the error message for the given error code.
        /// </summary>
        std::string message( int code ) const override;
    };

}  // namespace wincpp::core