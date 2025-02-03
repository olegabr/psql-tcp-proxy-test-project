/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "server_base_mock.hpp"
#include "acceptor_base_mock.hpp"

#include <io/socket.hpp>
#include <io/log.hpp>

#include <memory>

using socket_t = io::ip::tcp::socket;

io::test::server_base_mock::server_base_mock(io::bus_ptr io_bus)
    : _server_base(
          std::make_shared<io::test::acceptor_base_mock>(io_bus),
          [this](io::file_descriptor_t fd, const io::ip::v4 &address) -> io::ip::tcp::session_base_ptr
          {
              return _make_new_session(fd, address);
          })
{
}

io::ip::tcp::session_base_ptr io::test::server_base_mock::_make_new_session(io::file_descriptor_t fd, const io::ip::v4 &address)
{
    _latest_session = std::make_shared<io::test::session_base_mock>(_server_base.get_acceptor()->get_bus(), io::file_descriptors_vec_t{fd});
    return _latest_session;
}

io::ip::tcp::session_base_ptr io::test::server_base_mock::get_latest_session()
{
    return _latest_session;
}