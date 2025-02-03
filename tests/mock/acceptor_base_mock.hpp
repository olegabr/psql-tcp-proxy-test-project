/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_TCP_ACCEPTOR_T
#define H_IO_IP_TCP_ACCEPTOR_T

#include <io/v4.hpp>
#include <io/bus.hpp>
#include <io/acceptor_base.hpp>

#include <vector>
#include <memory>
#include <functional>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The test specific namespace
    namespace test
    {
        /// \brief The socket acceptor
        /// It listens on the address provided and accepts new connections
        /// accepted connections are reported with the callback function provided in constructor \ref acceptor::acceptor
        class acceptor_base_mock
            : public io::ip::acceptor_base
        {
        public:
            /// \brief Constructs new \ref acceptor object
            /// \param io_bus The \ref io::bus object instance to connect to the system level I/O
            /// \param callback The callback function the accepted connections are reported with
            acceptor_base_mock(
                io::bus_ptr io_bus,
                callback_t callback = nullptr);
            /// @brief The acceptor destructor
            ~acceptor_base_mock() noexcept override;

        private:
            /// \brief Handle the new client connection event
            /// @param reciever The async I/O event_reciever abstraction
            /// @param fd The file descriptor
            /// @param mask The I/O event mask
            /// @return The accepted client's address
            std::pair<io::file_descriptor_t, io::ip::v4> _accept_new_client(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask) override;
        };
    }
}

#endif // H_IO_IP_TCP_ACCEPTOR_T
