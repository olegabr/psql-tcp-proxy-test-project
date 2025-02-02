/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROTOCOL_TERMINATE_T
#define H_PSQL_PROTOCOL_TERMINATE_T

#include <cstddef> // std::byte
#include <cstdint> // std::size_t

/// @brief The general PostgreSQL related namespace
namespace psql
{
    /// \brief The Terminate message to the backend.
    /// The normal, graceful termination procedure is that the frontend sends a Terminate message and immediately closes the connection. On receipt of this message, the backend closes the connection and terminates.
    /// https://www.postgresql.org/docs/current/protocol-message-formats.html#PROTOCOL-MESSAGE-FORMATS-TERMINATE
    struct Terminate
    {
        /// \brief Identifies the message as a Terminate command.
        static constexpr std::byte MESSAGE_CODE = std::byte{'X'};
    };
    /// @brief Make the PostgreSQL \ref Terminate object
    /// @param payload The raw data buffer to construct message from
    /// @param payload_len The raw data buffer length to construct message from
    /// @return The \ref Terminate object created
    Terminate make_terminate_msg(const void *payload, const std::size_t payload_len);
}

#endif // H_PSQL_PROTOCOL_TERMINATE_T
