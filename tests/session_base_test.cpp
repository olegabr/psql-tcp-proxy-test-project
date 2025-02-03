/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include "mock/session_base_mock.hpp"
#include "mock/bus_mock.hpp"

#include <algorithm>
#include <sstream>

TEST(session_base, getters_start)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto sess = std::make_shared<io::test::session_base_mock>(bus, io::file_descriptors_vec_t{1});

    EXPECT_EQ(sess->get_bus().get(), bus.get());
    EXPECT_EQ(sess->get_file_descriptors().size(), 1);
    EXPECT_EQ(sess->get_file_descriptors().front(), 1);

    sess->base_start();

    bus->enqueue_event(1, io::flags::error);
    bus->wait_events(
        std::chrono::milliseconds{0},
        1);
}
