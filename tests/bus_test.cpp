/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/bus.hpp>
#include "mock/bus_mock.hpp"

#include <memory>
#include <stdexcept>

TEST(bus, add_fd)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
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
        -1,
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
    EXPECT_TRUE(callback_called);
}

TEST(bus, del_fd)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::in));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(1, io::flags::in);
    EXPECT_FALSE(callback_called);

    bus->del_fd(1);

    bool error_callback_called = false;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
    EXPECT_FALSE(callback_called);
}

TEST(bus, del_fd_callbacks)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::in));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(1, io::flags::in);
    EXPECT_FALSE(callback_called);

    bus->del_fd_callbacks(1);

    bool error_callback_called = false;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
    EXPECT_FALSE(callback_called);
}

TEST(bus, two_fds)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::in));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(1, io::flags::in);
    EXPECT_FALSE(callback_called);

    bool callback_called2 = false;
    bus->add_fd(
        2,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called2 = true;
            EXPECT_EQ(fd, 2);
            EXPECT_TRUE(mask.test(io::flags::out));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(2, io::flags::out);
    EXPECT_FALSE(callback_called2);

    bool error_callback_called = false;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
    EXPECT_TRUE(callback_called);
}

TEST(bus, error_event)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::error));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(1, io::flags::error);
    EXPECT_FALSE(callback_called);

    bool error_callback_called = false;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
    EXPECT_TRUE(callback_called);
}

TEST(bus, io_error_callback)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::error));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
            throw io::error("test error", fd, 12345);
        });
    bus->enqueue_event(1, io::flags::error);
    EXPECT_FALSE(callback_called);

    bool error_callback_called = false;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *reciever, const io::error &err)
        {
            error_callback_called = true;
            EXPECT_EQ(err.get_fd(), 1);
            EXPECT_EQ(err.get_errno(), 12345);
            EXPECT_STREQ(err.what(), "test error");
        });
    EXPECT_TRUE(error_callback_called);
    EXPECT_TRUE(callback_called);
}

TEST(bus, std_exception_callback)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::error));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
            throw std::runtime_error("test error");
        });
    bus->enqueue_event(1, io::flags::error);
    EXPECT_FALSE(callback_called);

    bool error_callback_called = false;
    errno = 0;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *reciever, const io::error &err)
        {
            error_callback_called = true;
            EXPECT_EQ(err.get_fd(), 1);
            EXPECT_EQ(err.get_errno(), 0);
            EXPECT_STREQ(err.what(), "test error");
        });
    EXPECT_TRUE(error_callback_called);
    EXPECT_TRUE(callback_called);
}

TEST(bus, unknown_exception_callback)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::error));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
            throw "test error";
        });
    bus->enqueue_event(1, io::flags::error);
    EXPECT_FALSE(callback_called);

    bool error_callback_called = false;
    errno = 0;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *reciever, const io::error &err)
        {
            error_callback_called = true;
            EXPECT_EQ(err.get_fd(), 1);
            EXPECT_EQ(err.get_errno(), 0);
            EXPECT_STREQ(err.what(), "unknown io error");
        });
    EXPECT_TRUE(error_callback_called);
    EXPECT_TRUE(callback_called);
}

///

TEST(bus, global_io_error_callback)
{
    auto bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::error));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(1, io::flags::error);
    EXPECT_FALSE(callback_called);

    bus->set_throw_io_error(true);

    bool error_callback_called = false;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *reciever, const io::error &err)
        {
            error_callback_called = true;
            EXPECT_EQ(err.get_fd(), -1);
            EXPECT_EQ(err.get_errno(), 12345);
            EXPECT_STREQ(err.what(), "test error");
        });
    EXPECT_TRUE(error_callback_called);
    EXPECT_FALSE(callback_called);
}

TEST(bus, global_std_exception_callback)
{
    auto bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::error));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(1, io::flags::error);
    EXPECT_FALSE(callback_called);

    bus->set_throw_std_exception(true);

    bool error_callback_called = false;
    errno = 0;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *reciever, const io::error &err)
        {
            error_callback_called = true;
            EXPECT_EQ(err.get_fd(), -1);
            EXPECT_EQ(err.get_errno(), 0);
            EXPECT_STREQ(err.what(), "test error");
        });
    EXPECT_TRUE(error_callback_called);
    EXPECT_FALSE(callback_called);
}

TEST(bus, global_unknown_exception_callback)
{
    auto bus = std::make_shared<io::test::bus_mock>();
    bool callback_called = false;
    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            callback_called = true;
            EXPECT_EQ(fd, 1);
            EXPECT_TRUE(mask.test(io::flags::error));
            EXPECT_EQ(reinterpret_cast<void *>(reciever), reinterpret_cast<void *>(bus.get()));
        });
    bus->enqueue_event(1, io::flags::error);
    EXPECT_FALSE(callback_called);

    bus->set_throw_unknown_error(true);

    bool error_callback_called = false;
    errno = 0;
    bus->wait_events(
        -1,
        1,
        [&](io::event_reciever *reciever, const io::error &err)
        {
            error_callback_called = true;
            EXPECT_EQ(err.get_fd(), -1);
            EXPECT_EQ(err.get_errno(), 0);
            EXPECT_STREQ(err.what(), "unknown io error");
        });
    EXPECT_TRUE(error_callback_called);
    EXPECT_FALSE(callback_called);
}
