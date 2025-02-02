/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "object_mock.hpp"
#include <algorithm>

io::test::object_base_mock::object_base_mock(io::bus_ptr bus, io::file_descriptor_t fd)
    : _bus(bus),
      _fd(fd),
      _throw_error(false),
      _result_buf_len(-1),
      _buffer(nullptr)
{
}

io::test::object_base_mock::~object_base_mock()
{
}

io::file_descriptor_t io::test::object_base_mock::_get_fd() const
{
    return _fd;
}

const io::bus_ptr &io::test::object_base_mock::_get_bus()
{
    return _bus;
}

void io::test::object_base_mock::set_throw_error(bool val)
{
    _throw_error = val;
}

void io::test::object_base_mock::set_result_buf_len(int val)
{
    _result_buf_len = val;
}

void io::test::object_base_mock::set_data_buffer(std::vector<std::byte> *buf)
{
    _buffer = buf;
}

///

io::test::input_object_mock::input_object_mock(io::bus_ptr bus, io::file_descriptor_t fd)
    : io::test::object_base_mock(bus, fd)
{
}

io::test::input_object_mock::~input_object_mock()
{
}

io::input_object::result_type io::test::input_object_mock::_async_read_some(value_t *buf, std::size_t buf_len)
{
    if (0ul == buf_len)
    {
        throw std::runtime_error("zero length buffer");
    }
    if (_throw_error)
    {
        return io::error("test error", get_fd(), 123);
    }
    std::size_t result_buf_len = buf_len / 2;
    if (0 == result_buf_len)
    {
        result_buf_len = 1;
    }
    if (-1 != _result_buf_len)
    {
        result_buf_len = _result_buf_len;
    }
    for (std::size_t i = 0; i < result_buf_len; ++i)
    {
        if (!_buffer || _buffer->size() <= i)
        {
            reinterpret_cast<std::byte *>(buf)[i] = std::byte{1};
        }
        else
        {
            reinterpret_cast<std::byte *>(buf)[i] = (*_buffer)[i];
        }
    }
    return success_result_type{get_fd(), buf, result_buf_len};
}

///

io::test::output_object_mock::output_object_mock(io::bus_ptr bus, io::file_descriptor_t fd)
    : io::test::object_base_mock(bus, fd)
{
}

io::test::output_object_mock::~output_object_mock()
{
}

io::output_object::result_type io::test::output_object_mock::_async_write_some(const value_t *buf, std::size_t buf_len)
{
    if (0ul == buf_len)
    {
        throw std::runtime_error("zero length buffer");
    }
    if (_throw_error)
    {
        return io::error("test error", get_fd(), 123);
    }
    std::size_t result_buf_len = buf_len / 2;
    if (0 == result_buf_len)
    {
        result_buf_len = 1;
    }
    if (-1 != _result_buf_len)
    {
        result_buf_len = _result_buf_len;
    }
    for (std::size_t i = 0; i < result_buf_len; ++i)
    {
        if (_buffer && i < _buffer->size())
        {
            (*_buffer)[i] = reinterpret_cast<const std::byte *>(buf)[i];
        }
    }
    return io::output_object::success_result_type{get_fd(), buf, result_buf_len};
}

///

io::test::io_object_mock::io_object_mock(io::bus_ptr bus, io::file_descriptor_t fd)
    : io::test::object_base_mock(bus, fd)
{
}
io::test::io_object_mock::~io_object_mock()
{
}

io::input_object::result_type io::test::io_object_mock::_async_read_some(value_t *buf, std::size_t buf_len)
{
    if (0ul == buf_len)
    {
        throw std::runtime_error("zero length buffer");
    }
    if (_throw_error)
    {
        return io::error("test error", get_fd(), 123);
    }
    std::size_t result_buf_len = buf_len / 2;
    if (0 == result_buf_len)
    {
        result_buf_len = 1;
    }
    if (-1 != _result_buf_len)
    {
        result_buf_len = _result_buf_len;
    }
    for (std::size_t i = 0; i < result_buf_len; ++i)
    {
        if (!_buffer || _buffer->size() <= i)
        {
            reinterpret_cast<std::byte *>(buf)[i] = std::byte{1};
        }
        else
        {
            reinterpret_cast<std::byte *>(buf)[i] = (*_buffer)[i];
        }
    }
    return io::input_object::success_result_type{get_fd(), buf, result_buf_len};
}

io::output_object::result_type io::test::io_object_mock::_async_write_some(const value_t *buf, std::size_t buf_len)
{
    if (0ul == buf_len)
    {
        throw std::runtime_error("zero length buffer");
    }
    if (_throw_error)
    {
        return io::error("test error", get_fd(), 123);
    }
    std::size_t result_buf_len = buf_len / 2;
    if (0 == result_buf_len)
    {
        result_buf_len = 1;
    }
    if (-1 != _result_buf_len)
    {
        result_buf_len = _result_buf_len;
    }
    for (std::size_t i = 0; i < result_buf_len; ++i)
    {
        if (_buffer && i < _buffer->size())
        {
            (*_buffer)[i] = reinterpret_cast<const std::byte *>(buf)[i];
        }
    }
    return io::output_object::success_result_type{get_fd(), buf, result_buf_len};
}
