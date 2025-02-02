/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "error.hpp"

io::error::error(const std::string &msg, file_descriptor_t fd, int errno_val)
    : std::runtime_error(msg), _fd(fd), _errno(errno_val)
{
}
// LCOV_EXCL_START
io::error::~error()
{
}
// LCOV_EXCL_STOP
