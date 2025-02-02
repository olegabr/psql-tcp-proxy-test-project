/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/flags.hpp>

#include <sstream>

TEST(io, flags_ostream)
{
    io::flags f{io::flags::error};
    std::stringstream s;

    EXPECT_TRUE(f.test(io::flags::error));

    s.str("");
    s << io::flags::empty;
    EXPECT_STREQ(s.str().c_str(), "[empty]");

    s.str("");
    s << io::flags::error;
    EXPECT_STREQ(s.str().c_str(), "[error]");

    s.str("");
    s << io::flags::in;
    EXPECT_STREQ(s.str().c_str(), "[in]");

    s.str("");
    s << io::flags::out;
    EXPECT_STREQ(s.str().c_str(), "[out]");

    s.str("");
    s << io::flags::inout;
    EXPECT_STREQ(s.str().c_str(), "[in][out]");
}
