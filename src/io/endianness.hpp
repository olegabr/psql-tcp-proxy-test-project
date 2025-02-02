/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_ENDIANNESS_T
#define H_IO_ENDIANNESS_T

#include <cstddef>
#include <cstdint>

/// \brief The input/output library namespace
namespace io
{
    /// @brief The endianness enumeration
    enum class endianness
    {
        /// @brief The little endian enumeration value
        LE,
        /// @brief The big endian enumeration value
        BE
    };

    /// @brief Get the uint32_t little endian value for the \p buffer provided
    /// @param buffer The buffer containing at least 4 bytes to parse uint32_t value from it.
    /// @return The uint32_t little endian value for the \p buffer provided
    /// @sa https://www.reddit.com/r/C_Programming/comments/pdgvm2/comment/has5aga/
    uint32_t decode_uint32_le(const std::byte *buffer);

    /// @brief Get the uint32_t big endian value for the \p buffer provided
    /// @param buffer The buffer containing at least 4 bytes to parse uint32_t value from it.
    /// @return The uint32_t big endian value for the \p buffer provided
    /// @sa https://www.reddit.com/r/C_Programming/comments/pdgvm2/comment/has5aga/
    uint32_t decode_uint32_be(const std::byte *buffer);

    /// @brief Get the uint32_t value interpreted according to the \p endianness for the \p buffer provided
    /// @param buffer The buffer containing at least 4 bytes to parse uint32_t value from it.
    /// @param endianness The endianness enumeration to interpret the \p buffer value in.
    /// @return The uint32_t value interpreted according to the \p endianness for the \p buffer provided
    /// @sa https://www.reddit.com/r/C_Programming/comments/pdgvm2/comment/has5aga/
    uint32_t decode_uint32(const std::byte *buffer, endianness endianness);

    /// @brief Get the uint16_t little endian value for the \p buffer provided
    /// @param buffer The buffer containing at least 4 bytes to parse uint16_t value from it.
    /// @return The uint16_t little endian value for the \p buffer provided
    /// @sa https://www.reddit.com/r/C_Programming/comments/pdgvm2/comment/has5aga/
    uint16_t decode_uint16_le(const std::byte *buffer);

    /// @brief Get the uint16_t big endian value for the \p buffer provided
    /// @param buffer The buffer containing at least 4 bytes to parse uint16_t value from it.
    /// @return The uint16_t big endian value for the \p buffer provided
    /// @sa https://www.reddit.com/r/C_Programming/comments/pdgvm2/comment/has5aga/
    uint16_t decode_uint16_be(const std::byte *buffer);

    /// @brief Get the uint16_t value interpreted according to the \p endianness for the \p buffer provided
    /// @param buffer The buffer containing at least 4 bytes to parse uint16_t value from it.
    /// @param endianness The endianness enumeration to interpret the \p buffer value in.
    /// @return The uint16_t value interpreted according to the \p endianness for the \p buffer provided
    /// @sa https://www.reddit.com/r/C_Programming/comments/pdgvm2/comment/has5aga/
    uint16_t decode_uint16(const std::byte *buffer, endianness endianness);
}

#endif // H_IO_ENDIANNESS_T
