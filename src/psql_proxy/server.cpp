/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "server.hpp"
#include "session.hpp"
#include "handler.hpp"

#include <io/acceptor.hpp>
#include <io/socket.hpp>
#include <io/log.hpp>

#include <iostream>

using socket_t = io::ip::tcp::socket;

psql_proxy::server::server(
	io::bus_ptr io_bus,
	const io::ip::v4 &address,
	const io::ip::v4 &target_address,
	int tcp_backlog,
	query_processor *qp)
	: _server_base(
		  std::make_shared<io::ip::tcp::acceptor>(io_bus, address, tcp_backlog),
		  [this](io::file_descriptor_t fd, const io::ip::v4 &address) -> io::ip::tcp::session_base_ptr
		  {
			  return _make_new_session(fd, address);
		  }),
	  _target_address(target_address),
	  _query_processor(qp)
{
	std::cout << "[+] Listening on " << address << std::endl;
	std::cout << "[+] Proxying to " << target_address << std::endl;
}

io::ip::tcp::session_base_ptr psql_proxy::server::_make_new_session(io::file_descriptor_t fd, const io::ip::v4 &address)
{
	std::cout << "[+] Got connection from: " << address << " --> fd: " << fd << "\n";
	auto from = std::make_shared<socket_t>(_server_base.get_acceptor()->get_bus(), fd);
	auto to = std::make_shared<socket_t>(_server_base.get_acceptor()->get_bus(), _target_address);
	return std::make_shared<psql_proxy::session>(from, to, _query_processor);
}
