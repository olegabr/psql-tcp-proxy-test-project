/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/acceptor.hpp>
#include <io/socket.hpp>
#include "mock/bus_mock.hpp"

TEST(acceptor, constructor)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::ip::v4 address{"127.0.0.1", "1234"};
    io::ip::tcp::acceptor acceptor(
        bus,
        address, 1024,
        [](io::file_descriptor_t fd, const io::ip::v4 &)
        {
            ;
        });

    EXPECT_EQ(acceptor.get_bus().get(), bus.get());
    EXPECT_NE(acceptor.get_fd(), 0);

    io::ip::tcp::socket sock(bus, address);
    EXPECT_EQ(sock.get_bus().get(), bus.get());
    EXPECT_NE(sock.get_fd(), 0);

    bus->enqueue_event(acceptor.get_fd(), io::flags::in);

    bool error_callback_called = false;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);

    bus->enqueue_event(acceptor.get_fd(), io::flags::error);

    error_callback_called = false;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_TRUE(error_callback_called);
}

TEST(acceptor, constructor_error)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::ip::v4 address{"123.123.123.123", "1234"};
    EXPECT_THROW((io::ip::tcp::acceptor{
                     bus,
                     address, 1024}),
                 io::error);
}
