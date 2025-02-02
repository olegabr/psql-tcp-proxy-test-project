/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "channel.hpp"
#include "log.hpp"

#include <iostream>
#include <algorithm> // std::copy

io::channel_ptr io::make_channel(
    const io::input_object_ptr &left,
    const io::output_object_ptr &right)
{
    auto ch = new io::channel(left, right);
    auto ch_ptr = io::channel_ptr(ch);
    ch->_start();
    return ch_ptr;
}

io::channel::channel(
    const io::input_object_ptr &left,
    const io::output_object_ptr &right)
    : _left(left),
      _right(right)
{
}

void io::channel::_start()
{
    _left->add_bus_callback(_make_left_socket_callback());
    _right->add_bus_callback(_make_right_socket_callback());
}

// LCOV_EXCL_START
io::channel::~channel()
{
    IO_DEBUG((std::cout << "~channel: from object id = " << _left->get_fd() << " to object id = " << _right->get_fd() << std::endl));
}
// LCOV_EXCL_STOP

void io::channel::add_handler(input_callback_t &&cb)
{
    _handlers.push_back(std::move(cb));
}

io::bus::callback_t io::channel::_make_left_socket_callback()
{
    auto self(shared_from_this());
    return [this, self](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
    {
        _handle_left_io_event(reciever, fd, mask);
    };
}

io::bus::callback_t io::channel::_make_right_socket_callback()
{
    auto self(shared_from_this());
    return [this, self](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
    {
        _handle_right_io_event(reciever, fd, mask);
    };
}

namespace
{
    /// @sa https://stackoverflow.com/a/49054086/1490653
    void print_bytes_hex(const void *buf, std::size_t length)
    {
#ifdef _IO_DEBUG_ENABLED
        std::cout << std::hex;
        auto cbuf = static_cast<const unsigned char *>(buf);
        for (std::size_t i = 0; i < length; ++i)
        {
            std::cout << (0xFF & cbuf[i]) << ' ';
        }
        std::cout << std::dec;
#endif // _IO_DEBUG_ENABLED
    }
}

void io::channel::_handle_left_io_event(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
{
    IO_DEBUG((std::cout << "channel::_handle_left_io_event: fd = " << fd << "; mask = " << mask << "; errno = " << errno << std::endl));
    if (mask.test(io::flags::error))
    {
        IO_DEBUG((std::cout << "io::channel read socket error: errno = " << errno << "\n"));
        // return;
    }
    if (mask.test(io::flags::in))
    {
        {
            auto wbuf = _buffer.write_acquire(CHUNK_SZ);
            if (nullptr != wbuf)
            {
                auto result = _left->async_read_some(wbuf, CHUNK_SZ);
                auto v = io::make_visitor{
                    [&](const io::error &err)
                    {
                        std::cerr << err.what() << "; errno = " << err.get_errno() << "; for fd = " << err.get_fd() << std::endl;
                        // connection closed - force error handler call to close the session
                        IO_DEBUG((std::cout << "channel::_handle_left_io_event: error from recv: fd = " << fd << "; mask = " << mask << "; errno = " << errno << std::endl));
                        reciever->enqueue_event(fd, io::flags::error);
                    },
                    [&](const io::input_object::success_result_type &res)
                    {
                        _buffer.write_release(res.buf_len);
                        IO_DEBUG((std::cout
                                  << "channel read io handler: fd = " << fd << "; recieved " << res.buf_len << " bytes:\n"));
                        print_bytes_hex(res.buf, res.buf_len);
                        IO_DEBUG((std::cout << std::endl));
                        IO_DEBUG((std::copy(static_cast<const char *>(res.buf), std::next(static_cast<const char *>(res.buf), res.buf_len), std::ostreambuf_iterator<char>(std::cout))));
                        IO_DEBUG((std::cout << std::endl));
                        // if (0ul == res.buf_len)
                        // {
                        //     switch (errno)
                        //     {
                        //     case EINTR:
                        //         break;
                        //     case EAGAIN:
                        //         break;
                        //     case EINPROGRESS:
                        //         break;
                        //     default:
                        //         // connection closed - force error handler call to close the session
                        //         IO_DEBUG((std::cout << "channel::_handle_left_io_event: zero from recv: fd = " << fd << "; mask = " << mask << "; errno = " << errno << std::endl));
                        //         reciever->enqueue_event(fd, io::flags::error);
                        //     }
                        // }
                        // else if (CHUNK_SZ == res.buf_len)
                        // {
                        //     // try to read more
                        //     reciever->enqueue_event(fd, io::flags::in);
                        // }
                    }};
                std::visit(v, result);
                std::for_each(_handlers.begin(), _handlers.end(), [&result](input_callback_t &handler)
                              { handler(result); });
            }
            else
            {
                IO_DEBUG((std::cout << "channel::_handle_left_io_event: write_acquire failed for fd = " << fd << "; mask = " << mask << "\n"));
            }
        }
        // try to write immediately if data recieved
        {
            auto [rbuf, len] = _buffer.read_acquire();
            if (nullptr != rbuf && len > 0)
            {
                auto result = _right->async_write_some(rbuf, len);
                size_t original_len = len;
                auto v = io::make_visitor{
                    [](const io::error &err)
                    {
                        std::cerr << err.what() << "; errno = " << err.get_errno() << "; for fd = " << err.get_fd() << std::endl;
                    },
                    [&](const io::output_object::success_result_type &res)
                    {
                        _buffer.read_release(res.buf_len);
                        IO_DEBUG((std::cout << "channel read io handler: fd = " << fd << "; sent " << res.buf_len << " bytes:\n"));
                        print_bytes_hex(res.buf, res.buf_len);
                        IO_DEBUG((std::cout << std::endl));
                        IO_DEBUG((std::copy(static_cast<const char *>(res.buf), std::next(static_cast<const char *>(res.buf), res.buf_len), std::ostreambuf_iterator<char>(std::cout))));
                        IO_DEBUG((std::cout << std::endl));
                        if (res.buf_len < original_len)
                        {
                            // try to write more
                            reciever->enqueue_event(fd, io::flags::out);
                        }
                    }};
                std::visit(v, result);
            }
        }
    }
}

void io::channel::_handle_right_io_event(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
{
    IO_DEBUG((std::cout << "channel::_handle_right_io_event: fd = " << fd << "; mask = " << mask << "; errno = " << errno << std::endl));
    if (mask.test(io::flags::error))
    {
        IO_DEBUG((std::cout << "io::channel write socket error: errno = " << errno << "\n"));
        // return;
    }
    if (mask.test(io::flags::out))
    {
        auto [rbuf, len] = _buffer.read_acquire();
        if (nullptr != rbuf && len > 0)
        {
            auto result = _right->async_write_some(rbuf, len);
            size_t original_len = len;
            auto v = io::make_visitor{
                [](const io::error &err)
                {
                    std::cerr << err.what() << "; errno = " << err.get_errno() << "; for fd = " << err.get_fd() << std::endl;
                },
                [&](const io::output_object::success_result_type &res)
                {
                    _buffer.read_release(res.buf_len);
                    IO_DEBUG((std::cout << "channel read io handler: fd = " << fd << "; sent " << res.buf_len << " bytes:\n"));
                    print_bytes_hex(res.buf, res.buf_len);
                    IO_DEBUG((std::cout << std::endl));
                    IO_DEBUG((std::copy(static_cast<const char *>(res.buf), std::next(static_cast<const char *>(res.buf), res.buf_len), std::ostreambuf_iterator<char>(std::cout))));
                    IO_DEBUG((std::cout << std::endl));
                    if (res.buf_len < original_len)
                    {
                        // try to write more
                        reciever->enqueue_event(fd, io::flags::out);
                    }
                }};
            std::visit(v, result);
        }
    }
}
