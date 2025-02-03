/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_TEST_SESSION_BASE_MOCK_T
#define H_IO_TEST_SESSION_BASE_MOCK_T

#include <io/session_base.hpp>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The test specific namespace
    namespace test
    {
        /// \brief The session base class for a TCP service
        class session_base_mock
            : public io::ip::tcp::session_base
        {
        public:
            /// \brief Construct the session base class for a TCP service
            /// \param bus The \ref io::bus object instance to connect to the system level I/O
            /// \param fds File descriptors related to this session
            session_base_mock(const io::bus_ptr &bus, io::file_descriptors_vec_t fds);
            /// \brief Destruct the session base class for a TCP service
            ~session_base_mock() noexcept override;
        };
    }
}
#endif // H_IO_TEST_SESSION_BASE_MOCK_T
