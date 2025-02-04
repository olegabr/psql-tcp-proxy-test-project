/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_ECHO_SESSION_T
#define H_ECHO_SESSION_T

#include <io/socket.hpp>
#include <io/channel.hpp>
#include <io/session_base.hpp>

#include <memory>

/// \brief The TCP echo service namespace
namespace echo
{
    /// \brief The session for the TCP echo service
    class session
        : public io::ip::tcp::session_base
    {
    public:
        /// \brief Create new echo session object
        /// \param socket The client socket
        explicit session(const io::ip::tcp::socket_ptr &socket);
        /// \brief Destroy the echo session object
        ~session() override;

    private:
        /// \brief from client to server channel
        io::channel_ptr _socket_pipe_lr;
        /// \brief from server to client channel
        io::channel_ptr _socket_pipe_rl;
    };
}

#endif // H_ECHO_SESSION_T
