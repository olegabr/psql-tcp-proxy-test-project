/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "bus_mock.hpp"
#include <io/error.hpp>

#include <algorithm> // std::for_each
#include <stdexcept>

io::test::bus_mock::bus_mock()
    : _do_throw_io_error(false),
      _do_throw_std_exception(false),
      _do_throw_unknown_error(false)
{
}

void io::test::bus_mock::set_throw_io_error(bool do_throw)
{
    _do_throw_io_error = do_throw;
}

void io::test::bus_mock::set_throw_std_exception(bool do_throw)
{
    _do_throw_std_exception = do_throw;
}

void io::test::bus_mock::set_throw_unknown_error(bool do_throw)
{
    _do_throw_unknown_error = do_throw;
}

io::test::bus_mock::~bus_mock() noexcept
{
}

void io::test::bus_mock::_add_fd(io::file_descriptor_t fd)
{
    _file_descriptors.insert(fd);
}

void io::test::bus_mock::_del_fd(io::file_descriptor_t fd)
{
    _file_descriptors.erase(fd);
}

void io::test::bus_mock::_wait_events(int timeout_msec, std::size_t events_buf_size, io::bus::callback_t callback)
{
    if (_do_throw_io_error)
    {
        throw io::error("test error", -1, 12345);
    }

    if (_do_throw_std_exception)
    {
        throw std::runtime_error("test error");
    }

    if (_do_throw_unknown_error)
    {
        throw "test error";
    }
}
