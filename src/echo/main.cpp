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
#include <exception>

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

/// @brief echo [HOST(127.0.0.1) [PORT(1234)]]
int main(int argc, char *argv[])
{
    signal(SIGINT, _cleanup);
    signal(SIGABRT, _cleanup);
    signal(SIGKILL, _cleanup);

    std::cout << "echo service start" << std::endl;

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

        const io::ip::v4 endpoint_address(host, port);
        const uint32_t tcp_backlog = 1024;
        echo::server tcp_server(
            io_bus,
            endpoint_address,
            tcp_backlog);

        auto error_handler = [](io::event_reciever *reciever, io::error const &ex)
        {
            std::cerr << "echo io error: " << ex.what()
                      << "; errno = " << ex.get_errno()
                      << "; for fd = " << ex.get_fd()
                      << std::endl;
        };

        io_context = std::make_shared<io::context>(io_bus, std::chrono::milliseconds{10});
        io_context->run(error_handler);

        std::cout << "echo service finish" << std::endl;
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
