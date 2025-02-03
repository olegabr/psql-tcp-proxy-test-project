/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_CONTEXT_T
#define H_IO_CONTEXT_T

#include "bus.hpp"

#include <cstddef> // std::size_t
#include <memory>

/// \brief The input/output library namespace
namespace io
{
	/// \brief The I/O reactor pattern object
	class context
	{
	public:
		/// @brief Construct the I/O reactor pattern object
		/// @param io_bus The shared pointer for an \ref io::bus object to listen on
		/// @param timeout_msec The maximum time to wait for events, in milliseconds. Or \ref std::chrono::milliseconds{0} for infinit wait.
		/// @param events_buf_size The events buffer size
		explicit context(const io::bus_ptr &io_bus, std::chrono::milliseconds timeout_msec = std::chrono::milliseconds{0}, std::size_t events_buf_size = 1024);

		/// @brief Start the event listening reactor pattern cycle
		/// @param error_callback The I/O bus async error callback
		void run(io::bus::error_callback_t error_callback = nullptr);
		/// @brief Stop the event listening reactor pattern cycle
		void stop();

		/// @brief Check if stop the event listening reactor pattern cycle is requested
		/// @return True if stop the event listening reactor pattern cycle is requested
		bool is_stop_requested() const
		{
			return _stop_requested;
		}

		/// \brief copy is prohibited
		context(const context &) = delete;
		/// \brief copy is prohibited
		context &operator=(const context &) = delete;

		/// \brief move is prohibited
		context(context &&) noexcept = delete;
		/// \brief move is prohibited
		context &operator=(context &&) noexcept = delete;

	private:
		/// @brief The shared pointer for an \ref io::bus object to listen on
		io::bus_ptr _io_bus;
		/// @brief True if stop the event listening reactor pattern cycle is requested
		bool _stop_requested;
		/// @brief The maximum time to wait for events, in milliseconds
		std::chrono::milliseconds _timeout_msec;
		/// @brief The events buffer size
		std::size_t _events_buf_size;
	};
	using context_ptr = std::shared_ptr<context>;
}

#endif // H_IO_CONTEXT_T
