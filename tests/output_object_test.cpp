/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include "mock/object_mock.hpp"
#include "mock/bus_mock.hpp"
#include <memory>

TEST(output_object, async_write_some)
{
    io::bus_ptr bus = std::make_shared<io::test::bus_mock>();
    auto obj = std::make_shared<io::test::output_object_mock>(bus, 1);

    std::byte data[] = {std::byte{1}};
    auto buf = reinterpret_cast<io::test::output_object_mock::value_t *>(data);
    std::size_t buf_len = 1;
    auto result = obj->async_write_some(buf, buf_len);

    bool callback_called = false;
    bool error_callback_called = false;
    auto v = io::make_visitor{
        [&](const io::error &err)
        {
            error_callback_called = true;
        },
        [&](const io::output_object::success_result_type &res)
        {
            callback_called = true;
            EXPECT_EQ(res.fd, 1);
            EXPECT_EQ(res.buf_len, 1);
            EXPECT_EQ(std::to_integer<int>(reinterpret_cast<const std::byte *>(res.buf)[0]), 1);
        }};
    std::visit(v, result);
    EXPECT_FALSE(error_callback_called);
    EXPECT_TRUE(callback_called);
}
