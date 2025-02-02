/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_ENDIANNESS_T
#define H_PSQL_PROXY_ENDIANNESS_T

#include <io/endianness.hpp>

#include <cstddef>
#include <cstdint>

/// @brief The general PostgreSQL related namespace
namespace psql
{
    /// @brief Detect the data endianness by euristics method:
    ///  SQL client request payload should not be too big
    /// @param buffer The buffer containing at least 4 bytes to parse uint32_t value from it.
    /// @return The \ref io::endianness enumeration value
    io::endianness check_endianness_by_uint32(const std::byte *buffer);
}

#endif // H_PSQL_PROXY_ENDIANNESS_T
