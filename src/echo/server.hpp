/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_ECHO_SERVER_T
#define H_ECHO_SERVER_T

#include "session.hpp"

#include <io/fd.hpp>
#include <io/v4.hpp>
#include <io/bus.hpp>
#include <io/session_manager.hpp>

#include <cstddef>
#include <memory>
#include <unordered_map>

/// \brief The TCP echo service namespace
namespace echo
{
	/// \brief The server for the TCP echo service
	class server final
	{
	public:
		/// \brief Create the server for the TCP echo service
		/// \param io_bus The \ref io::bus object instance to connect to the system level I/O
		/// \param address The \ref io::ip::v4 address like `127.0.0.1`
		/// \param tcp_backlog The TCP connections backlog value for the listening socket created
		server(
			io::bus_ptr io_bus,
			const io::ip::v4 &address,
			int tcp_backlog);

	private:
		/// \brief The function to create new \ref io::ip::tcp::session_base object for the \p fd
		/// \param fd The new client connection file descriptor
		/// \param address The new client connection address
		/// \return The \ref io::ip::tcp::session_base derived object for the newly created session
		io::ip::tcp::session_base_ptr _make_new_session(io::file_descriptor_t fd, const io::ip::v4 &address);

	private:
		/// \brief The TCP server class to manage new TCP sessions creation
		io::ip::tcp::session_manager _session_manager;
	};
}

#endif // H_ECHO_SERVER_T
