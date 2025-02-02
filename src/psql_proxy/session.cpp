/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "session.hpp"
#include "handler.hpp"

#include <io/log.hpp>

#include <iostream>
#include <iterator>

psql_proxy::session::session(
    const socket_ptr_t &socket,
    const socket_ptr_t &target_socket,
    query_processor *qp)
    : io::ip::tcp::session_base(socket->get_bus(), io::file_descriptors_vec_t{socket->get_fd(), target_socket->get_fd()}),
      _socket_pipe_lr(io::make_channel(socket, target_socket)),
      _socket_pipe_rl(io::make_channel(target_socket, socket))
{
    _socket_pipe_lr->add_handler(handler(qp, socket->get_fd(), socket->get_bus().get()));
}

psql_proxy::session::~session()
{
    std::cout << "[!] Client with fd: ";
    std::copy(get_file_descriptors().begin(), get_file_descriptors().end(),
              std::ostream_iterator<io::file_descriptor_t>(std::cout, ", "));
    std::cout << " has closed its connection"
              << "; errno = " << errno << "\n";
}