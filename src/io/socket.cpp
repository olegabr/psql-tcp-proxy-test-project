/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "socket.hpp"
#include "error.hpp"
#include "addrinfo.hpp"
#include "v4.hpp"
#include "log.hpp"

#include <iostream>
#include <algorithm>
#include <stdexcept>

#include <unistd.h> // ::close
#include <sys/socket.h>

io::ip::tcp::socket::socket(
    io::bus_ptr io_bus,
    file_descriptor_t fd)
    : _io_bus(io_bus),
      _fd(fd)
{
}

namespace
{
    io::file_descriptor_t _open_socket(const io::ip::system::addrinfo_t &addrinfo)
    {
        int sockfd = -1;

        io::ip::system::addrinfo_t::const_iterator p = addrinfo.begin();
        for (; p != addrinfo.end(); ++p)
        {
            sockfd = ::socket(p->ai_family, p->ai_socktype | SOCK_NONBLOCK, p->ai_protocol);
            if (sockfd == -1)
            {
                std::cerr << "failed to create socket\n";
                continue;
            }

            if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == -1 && errno != EINPROGRESS)
            {
                ::close(sockfd);
                std::cerr << "failed to connect socket\n";
                continue;
            }

            break;
        }

        if (p == addrinfo.end())
        {
            throw io::error("failed to connect", -1, errno);
        }

        return sockfd;
    }
}

io::ip::tcp::socket::socket(
    io::bus_ptr io_bus,
    const io::ip::v4 &address)
    : io::ip::tcp::socket(io_bus, _open_socket(io::ip::system::addrinfo_t(address)))
{
}

// LCOV_EXCL_START
io::ip::tcp::socket::~socket() noexcept
{
    IO_DEBUG((std::cout << "~socket: fd = " << _fd << std::endl));

    _close_connection_noexcept();
}
// LCOV_EXCL_STOP

io::file_descriptor_t io::ip::tcp::socket::_get_fd() const
{
    return _fd;
}

const io::bus_ptr &io::ip::tcp::socket::_get_bus()
{
    return _io_bus;
}

io::input_object::result_type io::ip::tcp::socket::_async_read_some(value_t *buf, std::size_t buf_len)
{
    try
    {
        const std::size_t bytes_recvd = _async_read_some_impl(buf, buf_len);
        return io::input_object::success_result_type{_fd, buf, bytes_recvd};
    }
    catch (io::error &err)
    {
        return err;
    }
}

std::size_t io::ip::tcp::socket::_async_read_some_impl(value_t *buf, std::size_t buf_len)
{
    _check_fd();

    errno = 0;
    const ssize_t bytes_recvd = ::recv(_fd, buf, buf_len, MSG_DONTWAIT);

    if (bytes_recvd == 0)
    {
        // // client disconnected
        // _close_connection();
        throw io::error("zero bytes read in recv", _fd, errno);
    }

    if (bytes_recvd == -1)
    {
        switch (errno)
        {
        case EINTR:
        case EAGAIN:
        case EINPROGRESS:
            return 0;
        default:
            IO_DEBUG((std::cout << "socket::_async_read_some_impl: recv() error. _close_connection()\n"));
            // _close_connection();
            throw io::error("recv failed", _fd, errno);
        }
    }
    return bytes_recvd;
}

io::output_object::result_type io::ip::tcp::socket::_async_write_some(const value_t *buf, std::size_t buf_len)
{
    try
    {
        const std::size_t bytes_sent = _async_write_some_impl(buf, buf_len);
        return io::output_object::success_result_type{_fd, buf, bytes_sent};
    }
    catch (io::error &err)
    {
        return err;
    }
}

std::size_t io::ip::tcp::socket::_async_write_some_impl(const value_t *buf, std::size_t buf_len)
{
    _check_fd();

    errno = 0;
    const ssize_t bytes_sent = ::send(_fd, buf, buf_len, MSG_DONTWAIT);

    if (bytes_sent == -1)
    {
        switch (errno)
        {
        case EINTR:
            return 0;
        case EAGAIN:
            return 0;
        case EINPROGRESS:
            return 0;
        default:
            IO_DEBUG((std::cout << "socket::_async_write_some_impl: send() error. _close_connection()\n"));
            // _close_connection();
            throw io::error("send failed", _fd, errno);
        }
    }
    return bytes_sent;
}

void io::ip::tcp::socket::_close_connection_noexcept() noexcept
{
    try
    {
        IO_DEBUG((std::cout << "socket::_close_connection_noexcept\n"));
        _close_connection();
    }
    catch (io::error &ex)
    {
        std::cerr << ex.what() << "; errno = " << ex.get_errno() << "; for fd = " << ex.get_fd() << std::endl;
    }
    catch (std::exception &ex)
    {
        std::cerr << "Fatal error: " << ex.what() << "; errno = " << errno << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown io error while closing socket connection" << "; errno = " << errno << std::endl;
    }
}

void io::ip::tcp::socket::_close_connection()
{
    if (-1 == _fd)
    {
        return;
    }

    IO_DEBUG((std::cout << "socket::_close_connection: fd = " << _fd << std::endl));
    // prevent re-enter and double close
    const io::file_descriptor_t fd = _fd;
    _fd = -1;

    try
    {
        _io_bus->del_fd(fd);
    }
    catch (...)
    {
        // failed to close connection. restore fd
        _fd = fd;
        throw;
    }
    if (-1 == ::shutdown(fd, SHUT_RDWR))
    {
        // failed to close connection. restore fd
        _fd = fd;
        throw io::error("failed to shutdown socket fd", fd, errno);
    }

    if (-1 == ::close(fd))
    {
        // failed to close connection. restore fd
        _fd = fd;
        throw io::error("failed to close socket fd", fd, errno);
    }
}

void io::ip::tcp::socket::_check_fd()
{
    if (-1 == _fd)
    {
        throw std::logic_error("closed socket used");
    }
}
