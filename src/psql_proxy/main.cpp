/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "server.hpp"
#include "query_processor.hpp"
#include "file_writer.hpp"

#include <io/error.hpp>
#include <io/epoll.hpp>
#include <io/context.hpp>

#include <iostream>
#include <signal.h>
#include <memory>
#include <chrono>
#include <thread>
#include <fstream>

namespace
{
    /// \brief The I/O reactor pattern object
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

/// @brief psql_proxy [PROXY_HOST(127.0.0.1) [PROXY_PORT(1235) [TARGET_HOST(127.0.0.1) [TARGET_PORT(5432) [QUERY_LOG_FILE_PATH(/tmp/query.log)]]]]]
int main(int argc, char *argv[])
{
    signal(SIGINT, _cleanup);
    signal(SIGABRT, _cleanup);
    signal(SIGKILL, _cleanup);

    std::cout << "psql_proxy service start" << std::endl;

    try
    {
        std::string host = "127.0.0.1";
        if (argc > 1)
        {
            host = argv[1];
        }

        std::string port = "1235";
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

        std::string query_log_path = "/tmp/query.log";
        if (argc > 5)
        {
            query_log_path = argv[5];
        }

        std::cout << "host: " << host << std::endl;
        std::cout << "port: " << port << std::endl;
        std::cout << "target_host: " << target_host << std::endl;
        std::cout << "target_port: " << target_port << std::endl;
        std::cout << "query_log_path: " << query_log_path << std::endl;

        /// \brief The endpoint this server is listening to
        const io::ip::v4 endpoint_address(host, port);
        /// \brief The address to proxy traffic to
        const io::ip::v4 target_address(target_host, target_port);
        /// \brief The tcp backlog queue length
        const uint32_t tcp_backlog = 1024;

        /// \brief The \ref io::bus implementation based on the GNU/Linux kernel epoll async I/O API.
        io::bus_ptr io_bus =
            std::make_shared<io::system::epoll>(EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLET);

        /// \brief The I/O reactor pattern object
        io_context = std::make_shared<io::context>(io_bus, std::chrono::milliseconds{10});

        /// @brief The PostgreSQL messages processor object
        psql_proxy::query_processor query_processor('\n');

        /// \brief The server for the PostgreSQL Proxy service
        psql_proxy::server tcp_server(
            io_bus,
            endpoint_address,
            target_address,
            tcp_backlog,
            &query_processor);

        /// @brief The file stream object to dump queries to.
        std::ofstream query_log_file(query_log_path, std::ios::trunc);
        psql_proxy::file_writer sql_queries_writer(io_context, &query_processor, &query_log_file);
        std::thread writer_thread(sql_queries_writer);

        auto error_handler = [](io::event_reciever *reciever, io::error const &ex)
        {
            std::cerr << "psql_proxy io error: " << ex.what()
                      << "; errno = " << ex.get_errno()
                      << "; for fd = " << ex.get_fd()
                      << std::endl;
        };
        io_context->run(error_handler);
        writer_thread.join();

        std::cout << "psql_proxy service finish" << std::endl;
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
