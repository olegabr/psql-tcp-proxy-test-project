/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "server_base.hpp"
#include "socket.hpp"

#include <iostream>
#include <utility>

using socket_t = io::ip::tcp::socket;

io::ip::tcp::server_base::server_base(const acceptor_ptr &tcp_acceptor)
    : _tcp_acceptor(tcp_acceptor)
{
    _tcp_acceptor->add_callback(
        [this](io::file_descriptor_t fd, const io::ip::v4 &address)
        {
            _accept_new_client(fd, address);
        });
}

// LCOV_EXCL_START
io::ip::tcp::server_base::~server_base() noexcept
{
}
// LCOV_EXCL_STOP

void io::ip::tcp::server_base::_accept_new_client(
    io::file_descriptor_t fd,
    const io::ip::v4 &address)
{
    _make_new_session(fd, address)->start();
}
