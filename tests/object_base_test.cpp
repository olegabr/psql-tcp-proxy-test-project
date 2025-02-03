/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include "mock/object_mock.hpp"
#include "mock/bus_mock.hpp"
#include <memory>

TEST(object_base, getters)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj = std::make_shared<io::test::object_base_mock>(bus, 1);
    EXPECT_EQ(obj->get_fd(), 1);
    EXPECT_EQ(obj->get_bus().get(), bus.get());
}

TEST(object_base, add_bus_callback)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj = std::make_shared<io::test::object_base_mock>(bus, 1);

    bool callback_called = false;
    obj->add_bus_callback(
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::in));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(1, io::flags::in);
    EXPECT_FALSE(callback_called);

    bool error_callback_called = false;
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
    EXPECT_TRUE(callback_called);
}

TEST(object_base, del_bus_fd_callbacks)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj = std::make_shared<io::test::object_base_mock>(bus, 1);

    bool callback_called = false;
    obj->add_bus_callback(
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
        });

    obj->del_bus_fd_callbacks();
    bus->enqueue_event(1, io::flags::in);
    EXPECT_FALSE(callback_called);

    bool error_callback_called = false;
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
    EXPECT_FALSE(callback_called);
}

TEST(object_base, del_bus_fd)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj = std::make_shared<io::test::object_base_mock>(bus, 1);

    bool callback_called = false;
    obj->add_bus_callback(
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
        });

    obj->del_bus_fd();
    bus->enqueue_event(1, io::flags::in);
    EXPECT_FALSE(callback_called);

    bool error_callback_called = false;
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
    EXPECT_FALSE(callback_called);
}
