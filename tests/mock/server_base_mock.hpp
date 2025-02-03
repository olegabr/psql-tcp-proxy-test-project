/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_TEST_SERVER_BASE_MOCK_T
#define H_IO_TEST_SERVER_BASE_MOCK_T

#include "session_base_mock.hpp"

#include <io/fd.hpp>
#include <io/v4.hpp>
#include <io/bus.hpp>
#include <io/server_base.hpp>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The test specific namespace
    namespace test
    {
        /// \brief The server for the TCP echo service
        class server_base_mock
            : public io::ip::tcp::server_base
        {
        public:
            /// \brief Create the server for the TCP echo service
            /// \param io_bus The \ref io::bus object instance to connect to the system level I/O
            explicit server_base_mock(io::bus_ptr io_bus);

            io::ip::tcp::session_base_ptr get_latest_session();

        private:
            /// \brief The function to create new \ref io::ip::tcp::session_base object for the \p fd
            /// \param fd The new client connection file descriptor
            /// \param address The new client connection address
            /// \return The \ref io::ip::tcp::session_base derived object for the newly created session
            io::ip::tcp::session_base_ptr _make_new_session(io::file_descriptor_t fd, const io::ip::v4 &address) override;

        private:
            io::ip::tcp::session_base_ptr _latest_session;
        };
    }
}
#endif // H_IO_TEST_SERVER_BASE_MOCK_T
