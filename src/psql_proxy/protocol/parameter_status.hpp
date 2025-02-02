/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROTOCOL_PARAMETER_STATUS_T
#define H_PSQL_PROTOCOL_PARAMETER_STATUS_T

#include <cstddef> // std::byte
#include <cstdint> // std::size_t
#include <vector>
#include <string>

/// @brief The general PostgreSQL related namespace
namespace psql
{
    /// @brief The PostgreSQL runtime parameter struct
    struct runtime_parameter
    {
        /// @brief The parameter name
        std::string name;
        /// @brief The parameter value
        std::string value;
    };

    /// @brief The PostgreSQL \ref ParameterStatus object.
    /// This message informs the frontend about the current (initial) setting of backend parameters,
    /// such as client_encoding or DateStyle.
    /// https://www.postgresql.org/docs/current/protocol-message-formats.html#PROTOCOL-MESSAGE-FORMATS-PARAMETERSTATUS
    struct ParameterStatus
    {
        /// \brief Identifies the message as a run-time parameter status report.
        static constexpr std::byte MESSAGE_CODE = std::byte{'S'};
        /// \brief At present there is a hard-wired set of parameters for which ParameterStatus will be generated. They are:
        /// application_name	is_superuser
        /// client_encoding	scram_iterations
        /// DateStyle	server_encoding
        /// default_transaction_read_only	server_version
        /// in_hot_standby	session_authorization
        /// integer_datetimes	standard_conforming_strings
        /// IntervalStyle	TimeZone
        std::vector<runtime_parameter> parameters;
    };
    /// @brief Make the PostgreSQL \ref ParameterStatus object
    /// @param payload The raw data buffer to construct message from
    /// @param payload_len The raw data buffer length to construct message from
    /// @return The \ref ParameterStatus object created
    ParameterStatus make_param_status_msg(const void *payload, const std::size_t payload_len);
}

#endif // H_PSQL_PROTOCOL_PARAMETER_STATUS_T
