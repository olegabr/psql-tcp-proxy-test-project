/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_TCP_PROXY_SESSION_T
#define H_TCP_PROXY_SESSION_T

#include <io/socket.hpp>
#include <io/channel.hpp>
#include <io/session_base.hpp>

#include <memory>

namespace tcp_proxy
{
    /// \brief The session for the TCP proxy service
    class session
        : public io::ip::tcp::session_base
    {
    public:
        /// \brief The \ref io::ip::tcp::socket_ptr alias type
        using socket_ptr_t = io::ip::tcp::socket_ptr;

        /// \brief Create new proxy session object
        /// \param socket The client socket
        /// \param target_socket The target socket
        session(
            const socket_ptr_t &socket,
            const socket_ptr_t &target_socket);
        /// \brief Destroy the proxy session object
        virtual ~session();

    private:
        /// \brief from client to server channel
        io::channel_ptr _socket_pipe_lr;
        /// \brief from server to client channel
        io::channel_ptr _socket_pipe_rl;
    };
}

#endif // H_TCP_PROXY_SESSION_T
