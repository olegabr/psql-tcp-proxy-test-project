/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_MESSAGE_T
#define H_PSQL_MESSAGE_T

#include <io/endianness.hpp>
#include "protocol/startup_message.hpp"
#include "protocol/parameter_status.hpp"
#include "protocol/query.hpp"
#include "protocol/terminate.hpp"

#include <cstddef> // std::byte
#include <cstdint> // std::size_t
#include <variant>
#include <optional>

/// @brief The general PostgreSQL related namespace
namespace psql
{
    /// @brief Make the PostgreSQL protocol message type
    using message = std::variant<
        StartupMessage,
        ParameterStatus,
        Query,
        Terminate>;

    /// @brief Make the PostgreSQL \ref Terminate object
    /// @param msg_code The PostgreSQL protocol message code
    /// @param payload The raw data buffer to construct message from
    /// @param payload_len The raw data buffer length to construct message from
    /// @param endianness The raw data endianness
    /// @return The \ref psql::message object if created or \ref std::nullopt
    std::optional<message> make_message(
        std::byte msg_code, const void *payload,
        const std::size_t payload_len,
        io::endianness endianness);
}

#endif // H_PSQL_MESSAGE_T
