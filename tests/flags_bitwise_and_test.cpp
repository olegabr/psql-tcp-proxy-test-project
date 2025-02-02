/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/flags.hpp>

TEST(io, flags_bitwise_and)
{
    io::flags f{io::flags::empty};
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::in;
    EXPECT_TRUE((f & io::flags::in).test(io::flags::in));
    EXPECT_TRUE((io::flags::in & f).test(io::flags::in));

    EXPECT_TRUE((f & io::flags::out).test(io::flags::empty));
    EXPECT_TRUE((io::flags::out & f).test(io::flags::empty));

    f = io::flags::out;
    EXPECT_TRUE((f & io::flags::out).test(io::flags::out));
    EXPECT_TRUE((io::flags::out & f).test(io::flags::out));

    EXPECT_TRUE((f & io::flags::in).test(io::flags::empty));
    EXPECT_TRUE((io::flags::in & f).test(io::flags::empty));

    f = io::flags::inout;
    EXPECT_TRUE((f & io::flags::inout).test(io::flags::inout));
    EXPECT_TRUE((io::flags::inout & f).test(io::flags::inout));

    EXPECT_TRUE((f & io::flags::in).test(io::flags::in));
    EXPECT_TRUE((io::flags::in & f).test(io::flags::in));

    EXPECT_TRUE((f & io::flags::out).test(io::flags::out));
    EXPECT_TRUE((io::flags::out & f).test(io::flags::out));

    f = io::flags::error;
    EXPECT_TRUE((f & io::flags::error).test(io::flags::error));
    EXPECT_TRUE((io::flags::error & f).test(io::flags::error));

    EXPECT_TRUE((f & io::flags::in).test(io::flags::empty));
    EXPECT_TRUE((io::flags::in & f).test(io::flags::empty));

    EXPECT_TRUE((f & io::flags::out).test(io::flags::empty));
    EXPECT_TRUE((io::flags::out & f).test(io::flags::empty));
}

TEST(io, flags_bitwise_assign_and)
{
    io::flags f{io::flags::empty};
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::empty;
    f &= io::flags::empty;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::empty;
    f &= io::flags::in;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::empty;
    f &= io::flags::out;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::empty;
    f &= io::flags::inout;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::empty;
    f &= io::flags::error;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::in;
    f &= io::flags::in;
    EXPECT_TRUE(f.test(io::flags::in));

    f = io::flags::in;
    f &= io::flags::out;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::in;
    f &= io::flags::inout;
    EXPECT_TRUE(f.test(io::flags::in));

    f = io::flags::in;
    f &= io::flags::empty;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::in;
    f &= io::flags::error;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::out;
    f &= io::flags::out;
    EXPECT_TRUE(f.test(io::flags::out));

    f = io::flags::out;
    f &= io::flags::in;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::out;
    f &= io::flags::inout;
    EXPECT_TRUE(f.test(io::flags::out));

    f = io::flags::out;
    f &= io::flags::empty;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::out;
    f &= io::flags::error;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::inout;
    f &= io::flags::inout;
    EXPECT_TRUE(f.test(io::flags::inout));

    f = io::flags::inout;
    f &= io::flags::out;
    EXPECT_TRUE(f.test(io::flags::out));

    f = io::flags::inout;
    f &= io::flags::in;
    EXPECT_TRUE(f.test(io::flags::in));

    f = io::flags::inout;
    f &= io::flags::empty;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::inout;
    f &= io::flags::error;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::error;
    f &= io::flags::error;
    EXPECT_TRUE(f.test(io::flags::error));

    f = io::flags::error;
    f &= io::flags::in;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::error;
    f &= io::flags::out;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::error;
    f &= io::flags::inout;
    EXPECT_TRUE(f.test(io::flags::empty));

    f = io::flags::error;
    f &= io::flags::empty;
    EXPECT_TRUE(f.test(io::flags::empty));
}
