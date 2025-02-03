/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/acceptor.hpp>
#include <io/socket.hpp>
#include <io/v4.hpp>
#include <io/epoll.hpp>

#include <memory>
#include <iostream>

TEST(epoll, constructor_native_callback)
{
    try
    {
        io::file_descriptor_t native_fd;
        io::system::epoll::event_mask_t native_mask;
        io::bus_ptr bus =
            std::make_shared<io::system::epoll>(
                EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLET,
                [&](io::file_descriptor_t fd, io::system::epoll::event_mask_t mask)
                {
                    native_fd = fd;
                    native_mask = mask;
                });
        // io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
        io::ip::v4 address{"127.0.0.1", "1234"};
        io::ip::tcp::socket_ptr sock;

        bool callback_called = false;
        bool callback_called3 = false;
        io::ip::tcp::acceptor acceptor(
            bus,
            address, 1024,
            [&](io::file_descriptor_t fd, const io::ip::v4 &)
            {
                callback_called = true;
                sock = std::make_shared<io::ip::tcp::socket>(bus, fd);

                sock->add_bus_callback(
                    [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
                    {
                        EXPECT_EQ(native_fd, fd);
                        EXPECT_TRUE(((mask.test(io::flags::in)) && (native_mask & EPOLLIN)) ||
                                    ((mask.test(io::flags::out)) && (native_mask & EPOLLOUT)) ||
                                    ((mask.test(io::flags::error)) && (native_mask & (EPOLLERR | EPOLLHUP))));
                        if (mask.test(io::flags::in))
                        {
                            callback_called3 = true;
                            std::byte data[] = {std::byte{0}};
                            auto buf = reinterpret_cast<io::input_object::value_t *>(data);
                            std::size_t buf_len = 1;
                            auto result = sock->async_read_some(buf, buf_len);
                        }
                    });
            });
        EXPECT_FALSE(callback_called);

        io::ip::tcp::socket client_sock(bus, address);
        // bus->enqueue_event(acceptor.get_fd(), io::flags::in);

        bool callback_called2 = false;
        client_sock.add_bus_callback(
            [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
            {
                EXPECT_EQ(native_fd, fd);
                EXPECT_TRUE(((mask.test(io::flags::in)) && (native_mask & EPOLLIN)) ||
                            ((mask.test(io::flags::out)) && (native_mask & EPOLLOUT)) ||
                            ((mask.test(io::flags::error)) && (native_mask & (EPOLLERR | EPOLLHUP))));
                if (mask.test(io::flags::out))
                {
                    callback_called2 = true;
                    std::byte data[] = {std::byte{0x0a}};
                    auto buf = reinterpret_cast<io::output_object::value_t *>(data);
                    std::size_t buf_len = 1;
                    auto result = client_sock.async_write_some(buf, buf_len);
                }
            });
        EXPECT_FALSE(callback_called2);
        // bus->enqueue_event(sock->get_fd(), io::flags::out);

        bool error_callback_called = false;
        bus->wait_events(
            std::chrono::milliseconds{0},
            1,
            [&](io::event_reciever *, const io::error &error)
            {
                error_callback_called = true;
                std::cerr << error.what() << "; errno = " << error.get_errno() << " for fd = " << error.get_fd() << std::endl;
            });
        EXPECT_FALSE(error_callback_called);
        EXPECT_TRUE(callback_called);

        bool error_callback_called2 = false;
        bus->wait_events(
            std::chrono::milliseconds{0},
            1,
            [&](io::event_reciever *, const io::error &error)
            {
                error_callback_called2 = true;
                std::cerr << error.what() << "; errno = " << error.get_errno() << " for fd = " << error.get_fd() << std::endl;
            });

        EXPECT_FALSE(error_callback_called2);
        EXPECT_TRUE(callback_called2);

        // bus->enqueue_event(sock->get_fd(), io::flags::in);

        bool error_callback_called3 = false;
        bus->wait_events(
            std::chrono::milliseconds{100},
            1,
            [&](io::event_reciever *, const io::error &error)
            {
                error_callback_called3 = true;
                std::cerr << error.what() << "; errno = " << error.get_errno() << " for fd = " << error.get_fd() << std::endl;
            });

        EXPECT_FALSE(error_callback_called3);
        EXPECT_TRUE(callback_called3);

        sock.reset();
    }
    catch (io::error &error)
    {
        std::cerr << error.what() << "; errno = " << error.get_errno() << " for fd = " << error.get_fd() << std::endl;
    }
}
