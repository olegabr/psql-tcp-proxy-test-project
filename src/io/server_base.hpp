/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_TCP_session_manager_T
#define H_IO_IP_TCP_session_manager_T

#include "fd.hpp"
#include "session_base.hpp"
#include "acceptor_base.hpp"

#include <functional>

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
            class session_manager final
            {
            public:
                /// \brief The function to create new \ref io::ip::tcp::session_base object for the \p fd
                /// \param fd The new client connection file descriptor
                /// \param address The new client connection address
                /// \return The \ref io::ip::tcp::session_base derived object for the newly created session
                using make_new_session_callback_t = std::function<session_base_ptr(io::file_descriptor_t fd, const io::ip::v4 &address)>;

                /// \brief Get the socket acceptor
                /// \return The socket acceptor
                acceptor_ptr get_acceptor()
                {
                    return _tcp_acceptor;
                }

                /// \brief Construct the server base class for a TCP service
                /// \param tcp_acceptor The socket acceptor
                explicit session_manager(const acceptor_ptr &tcp_acceptor, const make_new_session_callback_t &make_new_session_callback);
                /// \brief Destruct the server base class for a TCP service
                ~session_manager() noexcept;

            protected:
                /// \brief Server copying is prohibited
                session_manager(const session_manager &) = delete;
                /// \brief Server copying is prohibited
                session_manager &operator=(const session_manager &) = delete;

                /// \brief Server moving is prohibited
                session_manager(session_manager &&) noexcept = delete;
                /// \brief Server moving is prohibited
                session_manager &operator=(session_manager &&) noexcept = delete;

            private:
                /// \brief The socket acceptor
                acceptor_ptr _tcp_acceptor;
                make_new_session_callback_t _make_new_session_callback;
            };
        }
    }
}
#endif // H_IO_IP_TCP_session_manager_T
