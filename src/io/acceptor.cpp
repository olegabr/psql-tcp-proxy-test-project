/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "acceptor.hpp"
#include "error.hpp"
#include "log.hpp"

#include <string> // std::to_string
#include <iostream>

#include <unistd.h>		// ::close
#include <sys/socket.h> // ::shutdown
#include <arpa/inet.h>

namespace
{
	/// @brief Get a \ref sockaddr_in struct for this address
	/// @param address The address
	/// @return The \ref sockaddr_in struct for this address
	sockaddr_in _get_sockaddr_in(const io::ip::v4 &address)
	{
		sockaddr_in handler;
		handler.sin_family = AF_INET;
		handler.sin_addr.s_addr = ::inet_addr(address.host().c_str());
		handler.sin_port = ::htons(address.port());
		return handler;
	}

	// get sockaddr ip_v4
	const void *get_in_addr(const sockaddr *sa)
	{
		const sockaddr_in *sai = reinterpret_cast<const sockaddr_in *>(sa);
		return reinterpret_cast<const void *>(&(sai->sin_addr));
	}

	// get sockaddr port
	in_port_t get_in_port(const sockaddr *sa)
	{
		const sockaddr_in *sai = reinterpret_cast<const sockaddr_in *>(sa);
		return sai->sin_port;
	}

	/// @brief Construct the IPv4 address from \ref sockaddr_storage struct
	/// @param sa The \ref sockaddr_storage struct
	/// @return The address object constructed
	io::ip::v4 make_v4(sockaddr_storage const &sa)
	{
		std::array<char, INET_ADDRSTRLEN> conn_addr{0};
		inet_ntop(sa.ss_family, get_in_addr(reinterpret_cast<const sockaddr *>(&sa)), conn_addr.data(), conn_addr.size());

		std::string host = std::string(conn_addr.begin(), conn_addr.end());
		in_port_t port = get_in_port(reinterpret_cast<const sockaddr *>(&sa));
		std::string sport = std::to_string(port);
		return io::ip::v4(host, sport);
	}

	io::file_descriptor_t _open_socket()
	{
		io::file_descriptor_t sfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP);
		if (sfd == -1)
		{
			throw io::error("failed to open acceptor socket", sfd, errno);
		}
		return sfd;
	}

	class scope_guard_t
	{
	public:
		using callback_t = std::function<void()>;
		explicit scope_guard_t(callback_t cb)
			: _cb(cb)
		{
		}
		~scope_guard_t()
		{
			if (_cb)
			{
				_cb();
			}
		}

		void reset()
		{
			_cb = nullptr;
		}

	private:
		callback_t _cb;
	};
}

io::ip::tcp::acceptor::acceptor(
	io::bus_ptr io_bus,
	const io::ip::v4 &address,
	int tcp_backlog,
	callback_t callback)
	: io::ip::acceptor_base(io_bus, _open_socket()),
	  _endpoint_address(address),
	  _tcp_backlog(tcp_backlog)
{
	const file_descriptor_t sfd = get_fd();
	scope_guard_t scope_guard{
		[&]()
		{
			try
			{
				io::file_descriptor_t fd = get_fd();
				_close();
				::close(fd);
			}
			catch (std::exception &e)
			{
				std::cerr << e.what() << '\n';
			}
		}};

	// ensure smooth restarts after kill
	// @see https://stackoverflow.com/a/24194999/1490653
	const int enable = 1;
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		throw io::error("failed to reuse acceptor address", sfd, errno);
	}
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
	{
		throw io::error("failed to reuse acceptor port", sfd, errno);
	}

	sockaddr_in sa = _get_sockaddr_in(address);
	if (bind(sfd, reinterpret_cast<const sockaddr *>(&sa), sizeof(sa)) < 0)
	{
		throw io::error("failed to bind address to acceptor", sfd, errno);
	}

	if (listen(sfd, tcp_backlog) < 0)
	{
		throw io::error("failed to listen acceptor", sfd, errno);
	}

	add_callback(std::move(callback));
	scope_guard.reset();
}

// LCOV_EXCL_START
io::ip::tcp::acceptor::~acceptor() noexcept
{
	try
	{
		io::file_descriptor_t fd = get_fd();
		_close();
		::shutdown(fd, SHUT_RDWR);
		::close(fd);
	}
	catch (io::error &ex)
	{
		std::cerr << ex.what() << "; errno = " << ex.get_errno() << "; for fd = " << ex.get_fd() << '\n';
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
// LCOV_EXCL_STOP

std::pair<io::file_descriptor_t, io::ip::v4> io::ip::tcp::acceptor::_accept_new_client(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
{
	IO_DEBUG((std::cout << "acceptor::_accept_new_client: fd = " << fd << "; mask = " << mask << "; errno = " << errno << std::endl));
	if (mask.test(io::flags::error))
	{
		throw io::error("failed to accept", fd, errno);
	}
	sockaddr_storage clients;
	socklen_t clients_size = sizeof(clients);

	int conn = ::accept(get_fd(), reinterpret_cast<sockaddr *>(&clients), &clients_size);
	if (-1 == conn)
	{
		switch (errno)
		{
		case EINTR:
		case EAGAIN:
		case EINPROGRESS:
			reciever->enqueue_event(fd, io::flags::in);
			break;
		default:
			throw io::error("failed to accept", get_fd(), errno);
		}
	}
	else
	{
		// EPOLLET workaround
		reciever->enqueue_event(fd, io::flags::in);
	}

	io::ip::v4 address = make_v4(clients);
	return std::make_pair(conn, address);
}
