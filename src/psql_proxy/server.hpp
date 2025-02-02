/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_SERVER_T
#define H_PSQL_PROXY_SERVER_T

#include "session.hpp"
#include "query_processor.hpp"

#include <io/fd.hpp>
#include <io/v4.hpp>
#include <io/bus.hpp>
#include <io/acceptor.hpp>
#include <io/bipartite_buf.hpp>
#include <io/server_base.hpp>

#include <cstddef>
#include <memory>
#include <unordered_map>
#include <functional>

/// @brief The PostgreSQL Proxy service namespace
namespace psql_proxy
{
	/// \brief The server for the PostgreSQL Proxy service
	class server
		: public io::ip::tcp::server_base
	{
	public:
		/// \brief Create the server for the TCP proxy service
		/// \param io_bus The \ref io::bus object instance to connect to the system level I/O
		/// \param address The \ref io::ip::v4 address like `127.0.0.1`
		/// \param target_address The target address
		/// \param tcp_backlog The TCP connections backlog value for the listening socket created
		/// \param qp The PostgreSQL messages interpreter object
		server(
			io::bus_ptr io_bus,
			const io::ip::v4 &address,
			const io::ip::v4 &target_address,
			int tcp_backlog,
			query_processor *qp);

	private:
		/// \brief The function to create new \ref io::ip::tcp::session_base object for the \p fd
		/// \param fd The new client connection file descriptor
		/// \param address The new client connection address
		/// \return The \ref io::ip::tcp::session_base derived object for the newly created session
		virtual io::ip::tcp::session_base_ptr _make_new_session(io::file_descriptor_t fd, const io::ip::v4 &address);

	private:
		/// \brief The target address
		io::ip::v4 _target_address;
		/// \brief The PostgreSQL messages interpreter object
		query_processor *_query_processor;
	};
}

#endif // H_PSQL_PROXY_SERVER_T
