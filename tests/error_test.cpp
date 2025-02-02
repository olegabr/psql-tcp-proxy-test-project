/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/error.hpp>

#include <sstream>

TEST(error, construct)
{
    io::error err("message123", 123, 456);
    EXPECT_STREQ(err.what(), "message123");
    EXPECT_EQ(err.get_fd(), 123);
    EXPECT_EQ(err.get_errno(), 456);
}

TEST(error, copy_construct)
{
    io::error err0("message123", 123, 456);
    io::error err(err0);
    EXPECT_STREQ(err.what(), "message123");
    EXPECT_EQ(err.get_fd(), 123);
    EXPECT_EQ(err.get_errno(), 456);
}
