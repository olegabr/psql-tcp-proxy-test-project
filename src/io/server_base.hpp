/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_TCP_SERVER_BASE_T
#define H_IO_IP_TCP_SERVER_BASE_T

#include "fd.hpp"
#include "session_base.hpp"
#include "acceptor_base.hpp"

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
            class server_base
            {
            public:
                /// \brief Get the socket acceptor
                /// \return The socket acceptor
                acceptor_ptr get_acceptor()
                {
                    return _tcp_acceptor;
                }

            protected:
                /// \brief Construct the server base class for a TCP service
                /// \param tcp_acceptor The socket acceptor
                explicit server_base(const acceptor_ptr &tcp_acceptor);
                /// \brief Destruct the server base class for a TCP service
                virtual ~server_base() noexcept;

                /// \brief Server copying is prohibited
                server_base(const server_base &) = delete;
                /// \brief Server copying is prohibited
                server_base &operator=(const server_base &) = delete;

                /// \brief Server moving is prohibited
                server_base(server_base &&) noexcept = delete;
                /// \brief Server moving is prohibited
                server_base &operator=(server_base &&) noexcept = delete;

            private:
                /// \brief The function to create new \ref io::ip::tcp::session_base object for the \p fd
                /// \param fd The new client connection file descriptor
                /// \param address The new client connection address
                /// \return The \ref io::ip::tcp::session_base derived object for the newly created session
                virtual session_base_ptr _make_new_session(io::file_descriptor_t fd, const io::ip::v4 &address) = 0;

            private:
                /// \brief The callback function the accepted connections are reported with
                void _accept_new_client(io::file_descriptor_t fd, const io::ip::v4 &address);

            private:
                /// \brief The socket acceptor
                acceptor_ptr _tcp_acceptor;
            };
        }
    }
}
#endif // H_IO_IP_TCP_SERVER_BASE_T
