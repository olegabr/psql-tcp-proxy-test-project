/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_TCP_SESSION_BASE_T
#define H_IO_IP_TCP_SESSION_BASE_T

#include "bus.hpp"
#include "fd.hpp"

#include <memory>

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
            /// \brief The server base class for a TCP service
            class server_base;

            /// \brief The session base class for a TCP service
            class session_base
                : public std::enable_shared_from_this<session_base>
            {
            public:
                /// \brief Get file descriptors related to this session
                /// \return File descriptors related to this session
                const io::file_descriptors_vec_t &get_file_descriptors() const
                {
                    return _fds;
                }
                /// \brief get the \ref io::bus object instance to connect to the system level I/O
                /// \return The \ref io::bus object instance to connect to the system level I/O
                const io::bus_ptr &get_bus()
                {
                    return _bus;
                }

                /// \brief Session copying is prohibited
                session_base(const session_base &) = delete;
                /// \brief Session copying is prohibited
                session_base &operator=(const session_base &) = delete;

            protected:
                /// \brief Construct the session base class for a TCP service
                /// \param bus The \ref io::bus object instance to connect to the system level I/O
                /// \param fds File descriptors related to this session
                session_base(const io::bus_ptr &bus, io::file_descriptors_vec_t fds);
                /// \brief Destruct the session base class for a TCP service
                virtual ~session_base() noexcept;

            protected:
                friend class server_base;
                /// \brief Start the session: initialize \ref io::bus callbacks.
                // Can not do it in constructor because of the \ref std::enable_shared_from_this limitations
                void start();

            private:
                /// \brief The \ref io::bus object instance to connect to the system level I/O
                io::bus_ptr _bus;
                /// \brief File descriptors related to this session
                io::file_descriptors_vec_t _fds;
            };
            /// \brief The \ref session_base smart pointer alias
            using session_base_ptr = std::shared_ptr<session_base>;
        }
    }
}
#endif // H_IO_IP_TCP_SESSION_BASE_T
