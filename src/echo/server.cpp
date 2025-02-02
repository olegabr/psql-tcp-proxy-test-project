/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "server.hpp"
#include "session.hpp"

#include <io/acceptor.hpp>
#include <io/socket.hpp>
#include <io/log.hpp>

#include <iostream>
#include <utility>

using socket_t = io::ip::tcp::socket;

echo::server::server(
	io::bus_ptr io_bus,
	const io::ip::v4 &address,
	int tcp_backlog)
	: io::ip::tcp::server_base(std::make_shared<io::ip::tcp::acceptor>(io_bus, address, tcp_backlog))
{
	std::cout << "[+] Listening on " << address << std::endl;
}

io::ip::tcp::session_base_ptr echo::server::_make_new_session(io::file_descriptor_t fd, const io::ip::v4 &address)
{
	std::cout << "[+] Got connection from: " << address << " --> fd: " << fd << "\n";
	auto sock = std::make_shared<socket_t>(get_acceptor()->get_bus(), fd);
	return std::make_shared<echo::session>(sock);
}
