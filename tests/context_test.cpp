/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/context.hpp>
#include "mock/bus_mock.hpp"

TEST(context, run_stop_is_stop_requested)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::context ctx(bus);

    bus->add_fd(
        1,
        [&](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            ctx.stop();
        });
    bus->enqueue_event(1, io::flags::in);

    ctx.run();

    EXPECT_TRUE(ctx.is_stop_requested());
}

TEST(context, run_with_error_callback)
{
    auto bus = std::make_shared<io::test::bus_mock>();
    io::context ctx(bus);

    bus->add_fd(1);
    bus->enqueue_event(1, io::flags::in);

    bus->set_throw_io_error(true);

    bool error_callback_called = false;
    ctx.run(
        [&](io::event_reciever *, const io::error &)
        {
            ctx.stop();
            error_callback_called = true;
        });

    EXPECT_TRUE(error_callback_called);
}
