/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "endianness.hpp"

uint32_t io::decode_uint32_le(const std::byte *buffer)
{
    return (std::to_integer<uint32_t>(buffer[3]) << 24) |
           (std::to_integer<uint32_t>(buffer[2]) << 16) |
           (std::to_integer<uint32_t>(buffer[1]) << 8) |
           (std::to_integer<uint32_t>(buffer[0]));
}

uint32_t io::decode_uint32_be(const std::byte *buffer)
{
    return (std::to_integer<uint32_t>(buffer[0]) << 24) |
           (std::to_integer<uint32_t>(buffer[1]) << 16) |
           (std::to_integer<uint32_t>(buffer[2]) << 8) |
           (std::to_integer<uint32_t>(buffer[3]));
}

uint32_t io::decode_uint32(const std::byte *buffer, io::endianness endianness)
{
    if (io::endianness::LE == endianness)
    {
        return decode_uint32_le(buffer);
    }
    return decode_uint32_be(buffer);
}

uint16_t io::decode_uint16_le(const std::byte *buffer)
{
    return (std::to_integer<uint16_t>(buffer[1]) << 8) |
           (std::to_integer<uint16_t>(buffer[0]));
}

uint16_t io::decode_uint16_be(const std::byte *buffer)
{
    return (std::to_integer<uint16_t>(buffer[0]) << 8) |
           (std::to_integer<uint16_t>(buffer[1]));
}

uint16_t io::decode_uint16(const std::byte *buffer, io::endianness endianness)
{
    if (io::endianness::LE == endianness)
    {
        return decode_uint16_le(buffer);
    }
    return decode_uint16_be(buffer);
}
