/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_ERROR_T
#define H_IO_ERROR_T

#include "fd.hpp"

#include <string>
#include <stdexcept>

/// \brief The input/output library namespace
namespace io
{
    /// @brief The I/O operation error type
    class error
        : public std::runtime_error
    {
    public:
        /// @brief Construct the I/O operation error object
        /// @param msg The error message
        /// @param fd The file descriptor related to this I/O error
        /// @param errno_val The errno value related to this I/O error
        error(const std::string &msg, file_descriptor_t fd, int errno_val);
        ~error() override;

        /// @brief Get the file descriptor related to this I/O error
        /// @return The file descriptor related to this I/O error
        file_descriptor_t get_fd() const
        {
            return _fd;
        }
        /// @brief Get the errno value related to this I/O error
        /// @return The errno value related to this I/O error
        int get_errno() const
        {
            return _errno;
        }

    private:
        /// \brief The file descriptor related to this I/O error
        file_descriptor_t _fd;
        /// @brief The errno value related to this I/O error
        int _errno;
    };
}

#endif // H_IO_ERROR_T
