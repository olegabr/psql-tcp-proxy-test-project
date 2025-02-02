/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "endianness.hpp"

io::endianness psql::check_endianness_by_uint32(const std::byte *buffer)
{
    uint32_t le = io::decode_uint32_le(buffer);
    uint32_t be = io::decode_uint32_be(buffer);

    // endianness guess: SQL client request payload should not be big
    // if endianness is wrong, it would be big
    if (le <= be)
    {
        return io::endianness::LE;
    }
    return io::endianness::BE;
}
