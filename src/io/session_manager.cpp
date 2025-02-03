/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "session_manager.hpp"
#include "socket.hpp"

#include <iostream>
#include <utility>

using socket_t = io::ip::tcp::socket;

io::ip::tcp::session_manager::session_manager(const acceptor_ptr &tcp_acceptor, const make_new_session_callback_t &make_new_session_callback)
    : _tcp_acceptor(tcp_acceptor), _make_new_session_callback(make_new_session_callback)
{
    _tcp_acceptor->add_callback(
        [this](io::file_descriptor_t fd, const io::ip::v4 &address)
        {
            _make_new_session_callback(fd, address)->start();
        });
}

// LCOV_EXCL_START
io::ip::tcp::session_manager::~session_manager() noexcept
{
}
// LCOV_EXCL_STOP
