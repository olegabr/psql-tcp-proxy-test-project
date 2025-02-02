/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include "mock/acceptor_base_mock.hpp"
#include "mock/bus_mock.hpp"

TEST(acceptor_base, getters)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    io::test::acceptor_base_mock acceptor(bus);

    EXPECT_EQ(acceptor.get_bus().get(), bus.get());
    EXPECT_EQ(acceptor.get_fd(), 1);
}
