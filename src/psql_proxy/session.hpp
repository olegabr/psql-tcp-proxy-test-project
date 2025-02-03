/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_SESSION_T
#define H_PSQL_PROXY_SESSION_T

#include "query_processor.hpp"

#include <io/bus.hpp>
#include <io/socket.hpp>
#include <io/channel.hpp>
#include <io/session_base.hpp>

#include <memory>

/// @brief The PostgreSQL Proxy service namespace
namespace psql_proxy
{
    class session
        : public io::ip::tcp::session_base
    {
    public:
        using socket_ptr_t = io::ip::tcp::socket_ptr;

        session(
            const socket_ptr_t &socket,
            const socket_ptr_t &target_socket,
            query_processor *query_processor);
        ~session() final;

    private:
        io::channel_ptr _socket_pipe_lr;
        io::channel_ptr _socket_pipe_rl;
    };
}

#endif // H_PSQL_PROXY_SESSION_T
