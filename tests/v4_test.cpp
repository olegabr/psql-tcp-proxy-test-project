/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/v4.hpp>
#include <io/error.hpp>

#include <algorithm>
#include <sstream>

TEST(v4, construct)
{
    io::ip::v4 address("127.0.0.1", "5432");
    EXPECT_EQ(address.host(), "127.0.0.1");
    EXPECT_EQ(address.port(), 5432);
}

TEST(v4, copy_construct)
{
    io::ip::v4 address("127.0.0.1", "5432");
    io::ip::v4 address2(address);

    EXPECT_EQ(address2.host(), "127.0.0.1");
    EXPECT_EQ(address2.port(), 5432);

    EXPECT_EQ(address.host(), "127.0.0.1");
    EXPECT_EQ(address.port(), 5432);
}

TEST(v4, move_construct)
{
    io::ip::v4 address("127.0.0.1", "5432");
    io::ip::v4 address2(std::move(address));

    EXPECT_EQ(address2.host(), "127.0.0.1");
    EXPECT_EQ(address2.port(), 5432);

    EXPECT_EQ(address.host(), "");
    EXPECT_EQ(address.port(), 0);
}

TEST(v4, move_assignment)
{
    io::ip::v4 address("127.0.0.1", "5432");
    io::ip::v4 address2("127.0.0.2", "2345");
    address2 = std::move(address);

    EXPECT_EQ(address2.host(), "127.0.0.1");
    EXPECT_EQ(address2.port(), 5432);

    EXPECT_EQ(address.host(), "");
    EXPECT_EQ(address.port(), 0);
}

TEST(v4, self_assign)
{
    io::ip::v4 address("127.0.0.1", "5432");
    address = std::move(address);

    EXPECT_EQ(address.host(), "127.0.0.1");
    EXPECT_EQ(address.port(), 5432);
}

TEST(v4, output_operator)
{
    io::ip::v4 address("127.0.0.1", "5432");
    std::stringstream s;
    s.str("");
    s << address;
    EXPECT_STREQ(s.str().c_str(), "127.0.0.1:5432");
}
