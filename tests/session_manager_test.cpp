/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include "mock/session_manager_mock.hpp"
#include "mock/bus_mock.hpp"

TEST(session_manager, getters)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::test::session_manager_mock server(bus);
    EXPECT_EQ(server.get_acceptor()->get_bus().get(), bus.get());
}

TEST(session_manager, session_create)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::test::session_manager_mock server(bus);

    // imitate new client connection
    bus->enqueue_event(1, io::flags::in);
    bus->wait_events(
        std::chrono::milliseconds{0},
        1);

    auto sess = server.get_latest_session();
    EXPECT_NE(sess.get(), nullptr);

    EXPECT_EQ(sess->get_bus().get(), bus.get());
    EXPECT_EQ(sess->get_file_descriptors().size(), 1);
    EXPECT_EQ(sess->get_file_descriptors().front(), 1);
}
