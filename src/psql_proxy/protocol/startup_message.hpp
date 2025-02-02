/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROTOCOL_STARTUP_MESSAGE_T
#define H_PSQL_PROTOCOL_STARTUP_MESSAGE_T

#include <io/endianness.hpp>

#include <cstddef> // std::byte
#include <cstdint> // std::size_t
#include <vector>
#include <string>

/// @brief The general PostgreSQL related namespace
namespace psql
{
    /// @brief The PostgreSQL configuration parameter struct
    struct configuration_parameter
    {
        /// @brief The parameter name
        std::string name;
        /// @brief The parameter value
        std::string value;
    };

    /// @brief The PostgreSQL \ref StartupMessage object.
    /// To begin a session, a frontend opens a connection to the server and sends a startup message.
    /// This message includes the names of the user and of the database the user wants to connect to;
    /// it also identifies the particular protocol version to be used.
    /// https://www.postgresql.org/docs/current/protocol-message-formats.html#PROTOCOL-MESSAGE-FORMATS-STARTUPMESSAGE
    struct StartupMessage
    {
        /// For historical reasons, the very first message sent by the client (the startup message) has no initial message-type byte.
        static constexpr std::byte MESSAGE_CODE = std::byte{'\0'};

        /// \brief The protocol version number.
        union
        {
            int32_t _raw;
            struct
            {
                /// \brief The most significant 16 bits are the major version number (3 for the protocol described here).
                int16_t major;
                /// \brief The least significant 16 bits are the minor version number (0 for the protocol described here).
                int16_t minor;
            };
        } protocol_version;
        /// @brief The PostgreSQL configuration parameters
        std::vector<configuration_parameter> parameters;
    };
    /// @brief Make the PostgreSQL \ref StartupMessage object
    /// @param payload The raw data buffer to construct message from
    /// @param payload_len The raw data buffer length to construct message from
    /// @param endianness The raw data endianness
    /// @return The \ref StartupMessage object created
    StartupMessage make_startup_msg(const void *payload, const std::size_t payload_len, io::endianness endianness);
}

#endif // H_PSQL_PROTOCOL_STARTUP_MESSAGE_T
