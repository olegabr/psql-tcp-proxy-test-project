/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "session.hpp"
#include <io/log.hpp>
#include <iostream>
#include <iterator>
#include <algorithm>

echo::session::session(const io::ip::tcp::socket_ptr &socket)
    : io::ip::tcp::session_base(socket->get_bus(), io::file_descriptors_vec_t{socket->get_fd()}),
      _socket_pipe_lr(io::make_channel(socket, socket)),
      _socket_pipe_rl(io::make_channel(socket, socket))
{
}

echo::session::~session()
{
  std::cout << "[!] Client with fd: ";
  std::copy(get_file_descriptors().begin(), get_file_descriptors().end(),
            std::ostream_iterator<io::file_descriptor_t>(std::cout, ", "));
  std::cout << " has closed its connection"
            << "; errno = " << errno << "\n";
}
