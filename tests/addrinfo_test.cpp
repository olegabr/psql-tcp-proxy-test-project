/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/addrinfo.hpp>
#include <io/v4.hpp>
#include <io/error.hpp>

#include <algorithm>

TEST(addrinfo, construct)
{
    io::ip::v4 address("127.0.0.1", "5432");
    io::ip::system::addrinfo_t addrinfo(address);

    auto sz = std::distance(addrinfo.begin(), addrinfo.end());
    EXPECT_NE(sz, 0ul);

    auto p = addrinfo.begin();
    EXPECT_EQ(p->ai_family, AF_INET);
}

TEST(addrinfo, move_construct)
{
    io::ip::v4 address("127.0.0.1", "5432");
    io::ip::system::addrinfo_t addrinfo(address);
    io::ip::system::addrinfo_t addrinfo2(std::move(addrinfo));

    auto sz = std::distance(addrinfo2.begin(), addrinfo2.end());
    EXPECT_NE(sz, 0ul);

    auto p = addrinfo2.begin();
    EXPECT_EQ(p->ai_family, AF_INET);

    auto sz0 = std::distance(addrinfo.begin(), addrinfo.end());
    EXPECT_EQ(sz0, 0ul);
}

TEST(addrinfo, move_assignment)
{
    io::ip::v4 address("127.0.0.1", "5432");
    io::ip::system::addrinfo_t addrinfo(address);
    io::ip::system::addrinfo_t addrinfo2(address);
    addrinfo2 = std::move(addrinfo);

    auto sz = std::distance(addrinfo2.begin(), addrinfo2.end());
    EXPECT_NE(sz, 0ul);

    auto p = addrinfo2.begin();
    EXPECT_EQ(p->ai_family, AF_INET);

    auto sz0 = std::distance(addrinfo.begin(), addrinfo.end());
    EXPECT_EQ(sz0, 0ul);
}

TEST(addrinfo, self_assign)
{
    io::ip::v4 address("127.0.0.1", "5432");
    io::ip::system::addrinfo_t addrinfo(address);
    addrinfo = std::move(addrinfo);

    auto sz = std::distance(addrinfo.begin(), addrinfo.end());
    EXPECT_NE(sz, 0ul);

    auto p = addrinfo.begin();
    EXPECT_EQ(p->ai_family, AF_INET);
}

TEST(addrinfo, construct_error)
{
    io::ip::v4 address("do-not-exist", "54321");
    EXPECT_THROW((io::ip::system::addrinfo_t(address)), io::error);
}
