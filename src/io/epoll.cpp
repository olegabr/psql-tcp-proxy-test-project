/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "epoll.hpp"
#include "error.hpp"
#include "log.hpp"

#include <iostream>
#include <utility>

#include <unistd.h> // ::close
#include <sys/epoll.h>

io::system::epoll::epoll(event_mask_t event_mask, native_callback_t callback)
    : _epfd(0), _event_mask(event_mask), _native_callback(callback)
{
    int fd = epoll_create1(0);
    if (fd < 0)
    {
        throw io::error("failed to create epoll", fd, errno);
    }
    _epfd = fd;
}

// LCOV_EXCL_START
io::system::epoll::~epoll() noexcept
{
    IO_DEBUG((std::cout << "~epoll" << std::endl));
    if (-1 == ::close(_epfd))
    {
        std::cerr << "Failed to close epoll file descriptor: errno = " << errno << std::endl;
    }
    else
    {
        _epfd = 0;
    }
}
// LCOV_EXCL_STOP

void io::system::epoll::_add_fd(io::file_descriptor_t fd)
{
    struct epoll_event event = {};
    event.events = _event_mask;
    event.data.fd = fd;

    int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event);
    if (-1 == ret)
    {
        throw io::error("failed to add file descriptor to epoll", fd, errno);
    }
}

void io::system::epoll::_del_fd(io::file_descriptor_t fd)
{
    IO_DEBUG((std::cout << "Delete epoll file descriptor: fd = " << fd << std::endl));
    int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
    if (-1 == ret)
    {
        throw io::error("failed to delete file descriptor from epoll", fd, errno);
    }
}

namespace
{
    io::flags epoll_mask_to_io_flags(io::system::epoll::event_mask_t mask)
    {
        io::flags f = io::flags::empty;
        if (mask & (EPOLLERR | EPOLLHUP))
        {
            f |= io::flags::error;
        }
        if (mask & (EPOLLIN))
        {
            f |= io::flags::in;
        }
        if (mask & (EPOLLOUT))
        {
            f |= io::flags::out;
        }
        return f;
    }
}

void io::system::epoll::_wait_events(int timeout_msec, std::size_t events_buf_size, io::bus::callback_t callback)
{
    _events_buff.resize(events_buf_size);

    errno = 0;
    const int ret = ::epoll_wait(_epfd, _events_buff.data(), _events_buff.size(), timeout_msec);
    if (-1 == ret)
    {
        _events_buff.resize(0);
        if (errno != EINTR)
        {
            throw io::error("epoll wait error", _epfd, errno);
        }
    }
    else
    {
        _events_buff.resize(ret);
        for (const event_t &event : _events_buff)
        {
            if (_native_callback)
            {
                _native_callback(event.data.fd, event.events);
            }
            if (callback)
            {
                auto flags = epoll_mask_to_io_flags(event.events);
                IO_DEBUG((std::cout << "epoll::_wait_events: fd = " << event.data.fd << "; events = 0x" << std::hex << (0xFFFFFFFF & event.events) << std::dec << "; flags = " << flags << std::endl));
                if (flags.test(io::flags::error))
                {
                    IO_DEBUG((std::cout << "epoll::_wait_events error: fd = " << event.data.fd << "; events = 0x" << std::hex << (0xFFFFFFFF & event.events) << std::dec << std::endl));
                }
                callback(this, event.data.fd, flags);
            }
        }
    }
}
