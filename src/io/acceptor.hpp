/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_TCP_ACCEPTOR_T
#define H_IO_IP_TCP_ACCEPTOR_T

#include "v4.hpp"
#include "bus.hpp"
#include "object.hpp"
#include "acceptor_base.hpp"

#include <vector>
#include <memory>
#include <functional>

/// \brief The input/output library namespace
namespace io
{
	/// \brief The ip subnamespace of the input/output library
	namespace ip
	{

		/// \brief The ip::tcp subnamespace of the input/output library
		namespace tcp
		{
			/// \brief The socket acceptor
			/// It listens on the address provided and accepts new connections
			/// accepted connections are reported with the callback function provided in constructor \ref acceptor::acceptor
			class acceptor
				: public io::ip::acceptor_base
			{
			public:
				/// \brief Constructs new \ref acceptor object
				/// \param io_bus The \ref io::bus object instance to connect to the system level I/O
				/// \param address The \ref ip::v4 address like `127.0.0.1`
				/// \param tcp_backlog The TCP connections backlog value for the listening socket created
				/// \param callback The callback function the accepted connections are reported with
				acceptor(
					io::bus_ptr io_bus,
					const ip::v4 &address,
					int tcp_backlog,
					callback_t callback = nullptr);
				/// @brief The acceptor destructor
				virtual ~acceptor();

			private:
				/// \brief Handle the new client connection event
				/// @param reciever The async I/O event_reciever abstraction
				/// @param fd The file descriptor
				/// @param mask The I/O event mask
				/// @return The accepted client's address
				virtual std::pair<io::file_descriptor_t, io::ip::v4> _accept_new_client(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask);

			private:
				/// \brief address The \ref ip::v4 address like `127.0.0.1`
				io::ip::v4 _endpoint_address;
				/// \brief tcp_backlog The TCP connections backlog value for the listening socket created
				int _tcp_backlog;
			};
		}
	}
}

#endif // H_IO_IP_TCP_ACCEPTOR_T
