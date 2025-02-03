/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_TCP_SOCKET_T
#define H_IO_IP_TCP_SOCKET_T

#include "object.hpp"
#include "bus.hpp"
// #include "async_state.hpp"

#include <vector>
#include <memory>
#include <queue>

/// \brief The input/output library namespace
namespace io
{
	/// \brief The IP protocol related abstractions namespace
	namespace ip
	{
		class v4;
		/// \brief The TCP protocol related abstractions namespace
		namespace tcp
		{
			/// \brief The TCP socket abstraction
			class socket
				: public io::io_object
			{
			public:
				/// @brief Construct new \ref io::ip::tcp::socket object
				/// by wrapping the file descriptor provided
				/// @param io_bus The I/O bus this socket works on
				/// @param fd The already opened socket file descriptor
				socket(
					io::bus_ptr io_bus,
					file_descriptor_t fd);
				/// @brief Construct new \ref io::ip::tcp::socket object
				/// by opening socket from the \p address object provided
				/// @param io_bus The I/O bus this socket works on
				/// @param address The socket v4 address
				socket(
					io::bus_ptr io_bus,
					const io::ip::v4 &address);

				/// @brief Make sure the object is correctly destructed
				~socket() noexcept override;

				/// \brief copy is prohibited
				socket(const socket &) = delete;
				/// \brief copy is prohibited
				socket &operator=(const socket &) = delete;

				/// \brief move is prohibited
				socket(socket &&) noexcept = delete;
				/// \brief move is prohibited
				socket &operator=(socket &&) noexcept = delete;

			private:
				/// @brief Get the underlying file descriptor value for this socket
				/// @return The underlying file descriptor value for this socket
				io::file_descriptor_t _get_fd() const override;
				/// @brief Get the underlying \ref io::bus object for this socket
				/// @return The underlying \ref io::bus object for this socket
				const io::bus_ptr &_get_bus() override;

				/// @brief Read available data asynchronously.
				/// @param buf The buffer to write the recieved data to
				/// @param buf_len The length of the \p buf
				/// @return The \ref result_type with length of data read or error occured
				io::input_object::result_type _async_read_some(value_t *buf, std::size_t buf_len) override;
				/// @brief Write data asynchronously.
				/// @param buf The buffer of data to write from
				/// @param buf_len The length of the \p buf
				/// @return The \ref result_type with length of data written or error occured
				io::output_object::result_type _async_write_some(const value_t *buf, std::size_t buf_len) override;

			private:
				/// @brief Close this socket
				void _close_connection();
				/// @brief Close this socket with all exceptions catched.
				/// Useful in destructor
				void _close_connection_noexcept() noexcept;
				void _check_fd();

				/// @brief Read available data asynchronously.
				/// @param buf The buffer to write the recieved data to
				/// @param buf_len The length of the \p buf
				/// @return The read bytes count
				std::size_t _async_read_some_impl(value_t *buf, std::size_t buf_len);
				/// @brief Write data asynchronously.
				/// @param buf The buffer of data to write from
				/// @param buf_len The length of the \p buf
				/// @return The written bytes count
				std::size_t _async_write_some_impl(const value_t *buf, std::size_t buf_len);

			private:
				/// \brief The \ref io::bus object instance to connect to the system level I/O
				io::bus_ptr _io_bus;
				/// \brief This socket object file descriptor
				file_descriptor_t _fd;
			};
			/// \brief The TCP socket abstraction smart pointer
			using socket_ptr = std::shared_ptr<socket>;
		}
	}
}

#endif // H_IO_IP_TCP_SOCKET_T
