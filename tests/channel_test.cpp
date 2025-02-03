/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include "mock/object_mock.hpp"
#include "mock/bus_mock.hpp"
#include <io/channel.hpp>
#include <memory>

TEST(channel, getters)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::input_object_ptr obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    io::output_object_ptr obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    EXPECT_EQ(pipe->get_left()->get_fd(), 1);
    EXPECT_EQ(pipe->get_right()->get_fd(), 2);
}

TEST(channel, add_handler)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::input_object_ptr obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    io::output_object_ptr obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);
    pipe->add_handler(
        [&](const io::input_object::result_type &result)
        {
            ;
        });
}

TEST(channel, in_out_events_processing)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::input_object_ptr obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    io::output_object_ptr obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    bus->enqueue_event(1, io::flags::in);
    bus->enqueue_event(2, io::flags::out);
    bus->enqueue_event(2, io::flags::out);

    bool error_callback_called = false;
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
}

TEST(channel, error_events_processing)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::input_object_ptr obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    io::output_object_ptr obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    bus->enqueue_event(1, io::flags::error);
    bus->enqueue_event(2, io::flags::error);

    bool error_callback_called = false;
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
}

TEST(channel, error_read_events_processing)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    auto obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    obj1->set_throw_error(true);

    bus->enqueue_event(1, io::flags::in);
    bus->enqueue_event(2, io::flags::out);
    bus->enqueue_event(2, io::flags::out);

    bus->wait_events(
        std::chrono::milliseconds{0},
        1);
}

TEST(channel, error_write_events_processing)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    auto obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    obj2->set_throw_error(true);

    bus->enqueue_event(1, io::flags::in);
    bus->enqueue_event(2, io::flags::out);
    bus->enqueue_event(2, io::flags::out);

    bus->wait_events(
        std::chrono::milliseconds{0},
        1);
}

TEST(channel, in_out_events_processing_buffer_full)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    auto obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    obj1->set_result_buf_len(1024);

    bus->enqueue_event(1, io::flags::in);
    bus->enqueue_event(2, io::flags::out);
    bus->enqueue_event(2, io::flags::out);

    bool error_callback_called = false;
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &)
        {
            error_callback_called = true;
        });
    EXPECT_FALSE(error_callback_called);
}

TEST(channel, in_out_events_processing_connection_close)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    auto obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    obj1->set_result_buf_len(0);

    bus->enqueue_event(1, io::flags::in);
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &) {});
}

TEST(channel, in_out_events_processing_connection_close_EINTR)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    auto obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    obj1->set_result_buf_len(0);
    errno = EINTR;

    bus->enqueue_event(1, io::flags::in);
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &) {});
}

TEST(channel, in_out_events_processing_connection_close_EAGAIN)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    auto obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    obj1->set_result_buf_len(0);
    errno = EAGAIN;

    bus->enqueue_event(1, io::flags::in);
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &) {});
}

TEST(channel, in_out_events_processing_connection_close_EINPROGRESS)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    auto obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    obj1->set_result_buf_len(0);
    errno = EINPROGRESS;

    bus->enqueue_event(1, io::flags::in);
    bus->wait_events(
        std::chrono::milliseconds{0},
        1,
        [&](io::event_reciever *, const io::error &) {});
}

TEST(channel, check_data)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj1 = std::make_shared<io::test::input_object_mock>(bus, 1);
    auto obj2 = std::make_shared<io::test::output_object_mock>(bus, 2);
    auto pipe = io::make_channel(obj1, obj2);

    std::vector<std::byte> input_data{std::byte{0x0a}, std::byte{0x0b}, std::byte{0x0c}};
    std::vector<std::byte> output_data;
    output_data.resize(3, std::byte{0});

    obj1->set_data_buffer(&input_data);
    obj2->set_data_buffer(&output_data);

    obj1->set_result_buf_len(3);
    obj2->set_result_buf_len(3);

    bus->enqueue_event(1, io::flags::in);
    bus->enqueue_event(2, io::flags::out);

    bus->wait_events(std::chrono::milliseconds{0}, 1);

    EXPECT_EQ(output_data[0], std::byte{0x0a});
    EXPECT_EQ(output_data[1], std::byte{0x0b});
    EXPECT_EQ(output_data[2], std::byte{0x0c});
}
