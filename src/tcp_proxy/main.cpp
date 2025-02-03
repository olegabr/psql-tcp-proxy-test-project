/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "server.hpp"

#include <io/error.hpp>
#include <io/epoll.hpp>
#include <io/context.hpp>

#include <iostream>
#include <signal.h>
#include <memory>

namespace
{
    io::context_ptr io_context;
    void _cleanup(int signo)
    {
        std::cerr << "\nInterrupted with signal: " << signo << std::endl;
        if (io_context)
        {
            io_context->stop();
        }
    }
}

/// @brief tcp_proxy [PROXY_HOST(127.0.0.1) [PROXY_PORT(1234) [TARGET_HOST(127.0.0.1) [TARGET_PORT(5432)]]]]
int main(int argc, char *argv[])
{
    signal(SIGINT, _cleanup);
    signal(SIGABRT, _cleanup);
    signal(SIGKILL, _cleanup);

    std::cout << "tcp_proxy service start" << std::endl;

    try
    {
        io::bus_ptr io_bus =
            std::make_shared<io::system::epoll>(EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLET);

        std::string host = "127.0.0.1";
        if (argc > 1)
        {
            host = argv[1];
        }

        std::string port = "1234";
        if (argc > 2)
        {
            port = argv[2];
        }

        std::string target_host = "127.0.0.1";
        if (argc > 3)
        {
            target_host = argv[3];
        }

        std::string target_port = "5432";
        if (argc > 4)
        {
            target_port = argv[4];
        }
        const io::ip::v4 endpoint_address(host, port);
        const io::ip::v4 target_address(target_host, target_port);
        const uint32_t tcp_backlog = 1024;
        tcp_proxy::server tcp_server(
            io_bus,
            endpoint_address,
            target_address,
            tcp_backlog);

        auto error_handler = [](io::event_reciever *reciever, io::error const &ex)
        {
            std::cerr << "tcp_proxy io error: " << ex.what()
                      << "; errno = " << ex.get_errno()
                      << "; for fd = " << ex.get_fd()
                      << std::endl;
        };

        io_context = std::make_shared<io::context>(io_bus, std::chrono::milliseconds{10});
        io_context->run(error_handler);

        std::cout << "tcp_proxy service finish" << std::endl;
    }
    catch (io::error &ex)
    {
        std::cerr << "Fatal error: " << ex.what() << "; errno = " << ex.get_errno() << "; for fd = " << ex.get_fd() << std::endl;
        exit(1);
    }
    catch (std::exception &ex)
    {
        std::cerr << "Fatal error: " << ex.what() << "; errno = " << errno << std::endl;
        exit(1);
    }
    catch (...)
    {
        std::cerr << "Unknown io error. errno = " << errno << std::endl;
        exit(1);
    }

    return 0;
}
