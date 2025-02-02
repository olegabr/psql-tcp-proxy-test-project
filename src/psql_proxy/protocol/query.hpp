/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROTOCOL_QUERY_T
#define H_PSQL_PROTOCOL_QUERY_T

#include <cstddef> // std::byte
#include <cstdint> // std::size_t
#include <string>

/// @brief The general PostgreSQL related namespace
namespace psql
{
    /// \brief The Query message to the backend.
    /// The message includes an SQL command (or commands) expressed as a text string.
    /// https://www.postgresql.org/docs/current/protocol-message-formats.html#PROTOCOL-MESSAGE-FORMATS-QUERY
    struct Query
    {
        /// \brief Identifies the message as a Query command.
        static constexpr std::byte MESSAGE_CODE = std::byte{'Q'};

        /// \brief The query string itself.
        std::string query;
    };
    /// @brief Make the PostgreSQL \ref Query object
    /// @param payload The raw data buffer to construct message from
    /// @param payload_len The raw data buffer length to construct message from
    /// @return The \ref Query object created
    Query make_query_msg(const void *payload, const std::size_t payload_len);
}

#endif // H_PSQL_PROTOCOL_QUERY_T
