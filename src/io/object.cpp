/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "object.hpp"
#include "log.hpp"

#include <algorithm>
#include <iostream>

io::file_descriptor_t io::object_base::get_fd() const
{
    return _get_fd();
}
const io::bus_ptr &io::object_base::get_bus()
{
    return _get_bus();
}
void io::object_base::add_bus_callback(const io::bus::callback_t &cb)
{
    _get_bus()->add_fd(_get_fd(), cb);
}
void io::object_base::del_bus_fd_callbacks()
{
    _get_bus()->del_fd_callbacks(_get_fd());
}
void io::object_base::del_bus_fd()
{
    IO_DEBUG((std::cout << "object_base::del_bus_fd: fd = " << _get_fd() << std::endl));
    _get_bus()->del_fd(_get_fd());
}

// LCOV_EXCL_START
io::object_base::~object_base() {}
// LCOV_EXCL_STOP

io::input_object::result_type io::input_object::async_read_some(value_t *buf, std::size_t buf_len)
{
    return _async_read_some(buf, buf_len);
}

// LCOV_EXCL_START
io::input_object::~input_object() {}
// LCOV_EXCL_STOP

io::output_object::result_type io::output_object::async_write_some(const value_t *buf, std::size_t buf_len)
{
    return _async_write_some(buf, buf_len);
}

// LCOV_EXCL_START
io::output_object::~output_object() {}
// LCOV_EXCL_STOP

// LCOV_EXCL_START
io::io_object::~io_object() {}
// LCOV_EXCL_STOP
