/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_EPOLL_T
#define H_IO_EPOLL_T

#include "bus.hpp"

#include <cstddef>
#include <vector>
#include <functional>

#include <sys/epoll.h> // struct epoll_event

/// \brief The input/output library namespace
namespace io
{
	/// \brief The system specific namespace
	namespace system
	{
		/// \brief The \ref io::bus implementation based on the GNU/Linux kernel epoll async I/O API.
		class epoll
			: public io::bus
		{
		public:
			/// \brief The native epoll event mask type.
			using event_mask_t = int;
			/// \brief The native epoll event type.
			using event_t = struct epoll_event;
			/// @brief The native epoll callback.
			/// Accepts \ref file_descriptor_t and \ref event_mask_t
			using native_callback_t = std::function<void(file_descriptor_t, event_mask_t)>;

			/// @brief Constructs new epoll \ref io::bus implementation object.
			/// @param event_mask The native epoll event mask.
			/// @param callback The native epoll callback.
			explicit epoll(event_mask_t event_mask, native_callback_t callback = nullptr);
			/// @brief Make sure the object is correctly destructed
			~epoll() noexcept override;

			/// \brief copy is prohibited
			epoll(const epoll &) = delete; // non construction-copyable
			/// \brief copy is prohibited
			epoll &operator=(const epoll &) = delete; // non copyable

			/// \brief move is prohibited
			epoll(epoll &&) noexcept = delete;
			/// \brief move is prohibited
			epoll &operator=(epoll &&) noexcept = delete;

		private:
			/// @brief Listen on the \p fd file descriptor.
			/// The \ref io::bus::_add_fd pure virtual function implementation.
			/// @param fd The file descriptor
			void _add_fd(file_descriptor_t fd) override;
			/// @brief Stop listening on the \p fd file descriptor
			/// The \ref io::bus::_del_fd pure virtual function implementation.
			/// @param fd The file descriptor
			void _del_fd(file_descriptor_t fd) override;

			/// @brief Wait for I/O events on this bus object.
			/// It waits for the \p timeout_msec milliseconds or
			/// while the \p events_buf_size events is read.
			/// The \ref io::bus::_wait_events pure virtual function implementation.
			/// @param timeout_msec The maximum time to wait for events, in milliseconds. Or \ref std::chrono::milliseconds{0} for infinit wait.
			/// @param events_buf_size The events buffer size
			/// @param callback The I/O bus async event callback function to be called for each I/O event triggered.
			void _wait_events(std::chrono::milliseconds timeout_msec, std::size_t events_buf_size, io::bus::callback_t callback) override;

		private:
			/// \brief The epoll file descriptor.
			int _epfd;

			/// \brief The native epoll events buffer type.
			using event_buff_t = std::vector<event_t>;
			/// \brief The native epoll events buffer.
			event_buff_t _events_buff;

			/// \brief The native epoll event mask
			event_mask_t _event_mask;
			/// @brief The native epoll callback
			native_callback_t _native_callback;
		};
	}
}

#endif // H_IO_EPOLL_T
