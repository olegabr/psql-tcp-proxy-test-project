/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include <gtest/gtest.h>
#include <io/endianness.hpp>

TEST(endianness, decode_uint32_le)
{
    /// https://stackoverflow.com/a/41766046/1490653
    const std::byte b[]{std::byte{0x61}, std::byte{0x62}, std::byte{0x63}, std::byte{0x64}};
    uint32_t i = io::decode_uint32_le(b);
    EXPECT_EQ(i, 0x64636261);
}

TEST(endianness, decode_uint32_be)
{
    const std::byte b[]{std::byte{0x61}, std::byte{0x62}, std::byte{0x63}, std::byte{0x64}};
    uint32_t i = io::decode_uint32_be(b);
    EXPECT_EQ(i, 0x61626364);
}

TEST(endianness, decode_uint32_le_general)
{
    const std::byte b[]{std::byte{0x61}, std::byte{0x62}, std::byte{0x63}, std::byte{0x64}};
    uint32_t i = io::decode_uint32(b, io::endianness::LE);
    EXPECT_EQ(i, 0x64636261);
}

TEST(endianness, decode_uint32_be_general)
{
    const std::byte b[]{std::byte{0x61}, std::byte{0x62}, std::byte{0x63}, std::byte{0x64}};
    uint32_t i = io::decode_uint32(b, io::endianness::BE);
    EXPECT_EQ(i, 0x61626364);
}

/// 16 bit

TEST(endianness, decode_uint16_le)
{
    const std::byte b[]{std::byte{0x61}, std::byte{0x62}};
    uint16_t i = io::decode_uint16_le(b);
    EXPECT_EQ(i, 0x6261);
}

TEST(endianness, decode_uint16_be)
{
    const std::byte b[]{std::byte{0x61}, std::byte{0x62}};
    uint16_t i = io::decode_uint16_be(b);
    EXPECT_EQ(i, 0x6162);
}

TEST(endianness, decode_uint16_le_general)
{
    const std::byte b[]{std::byte{0x61}, std::byte{0x62}};
    uint16_t i = io::decode_uint16(b, io::endianness::LE);
    EXPECT_EQ(i, 0x6261);
}

TEST(endianness, decode_uint16_be_general)
{
    const std::byte b[]{std::byte{0x61}, std::byte{0x62}};
    uint16_t i = io::decode_uint16(b, io::endianness::BE);
    EXPECT_EQ(i, 0x6162);
}
