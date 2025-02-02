/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_ACCEPTOR_BASE_T
#define H_IO_IP_ACCEPTOR_BASE_T

#include "bus.hpp"
#include "object.hpp"

#include <vector>
#include <memory>
#include <functional>
#include <utility>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The ip subnamespace of the input/output library
    namespace ip
    {
        class v4;
        /// \brief The incoming connection acceptor base class
        /// It manages callback functions
        class acceptor_base
            : public io::object_base
        {
        public:
            /// \brief The callback function type the accepted connections are reported with
            using callback_t = std::function<void(file_descriptor_t, const io::ip::v4 &)>;

            /// \brief Function to add more callbacks after the object already created
            /// \param callback The callback function the accepted connections are reported with
            void add_callback(callback_t callback);

        protected:
            /// \brief Constructs new \ref acceptor_base object
            /// \param io_bus The \ref io::bus object instance to connect to the system level I/O
            /// @param fd The already opened socket file descriptor
            explicit acceptor_base(const io::bus_ptr &io_bus, io::file_descriptor_t fd);
            /// @brief The acceptor destructor
            virtual ~acceptor_base();

            /// @brief remove file descriptor from \ref io::bus
            void _close();

            /// \brief copy is prohibited
            acceptor_base(const acceptor_base &) = delete; // non construction-copyable
            /// \brief copy is prohibited
            acceptor_base &operator=(const acceptor_base &) = delete; // non copyable

            /// \brief move is prohibited
            acceptor_base(acceptor_base &&) noexcept = delete;
            /// \brief move is prohibited
            acceptor_base &operator=(acceptor_base &&) noexcept = delete;

        private:
            /// @brief Get the underlying file descriptor value for this socket
            /// @return The underlying file descriptor value for this socket
            virtual file_descriptor_t _get_fd() const;
            /// @brief Get the underlying \ref io::bus object for this socket
            /// @return The underlying \ref io::bus object for this socket
            virtual const io::bus_ptr &_get_bus();

        private:
            /// \brief Handle the new client connection event
            /// @param reciever The async I/O event_reciever abstraction
            /// @param fd The file descriptor
            /// @param mask The I/O event mask
            /// @return The accepted client's address
            virtual std::pair<io::file_descriptor_t, io::ip::v4> _accept_new_client(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask) = 0;

        private:
            /// \brief The callbacks collection type
            using callback_vec_t = std::vector<callback_t>;

            /// \brief The \ref io::bus object instance to connect to the system level I/O
            io::bus_ptr _io_bus;
            /// \brief The listening socket file descriptor
            file_descriptor_t _fd;
            /// \brief Callback functions the accepted connections are reported with
            callback_vec_t _callbacks;
        };
        /// \brief The \ref io::ip::tcp::acceptor smart pointer alias
        using acceptor_ptr = std::shared_ptr<acceptor_base>;
    }
}

#endif // H_IO_IP_ACCEPTOR_BASE_T
