/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_FD_T
#define H_IO_FD_T

#include <vector>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The file descriptor type
    using file_descriptor_t = int;
    /// \brief The file descriptors collection type
    using file_descriptors_vec_t = std::vector<io::file_descriptor_t>;
}

#endif // H_IO_FD_T
