/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "message.hpp"

std::optional<psql::message> psql::make_message(std::byte msg_code, const void *payload, const std::size_t payload_len, io::endianness endianness)
{
    std::optional<psql::message> msg;

    switch (msg_code)
    {
    case psql::StartupMessage::MESSAGE_CODE:
        msg = make_startup_msg(payload, payload_len, endianness);
        break;
    case psql::ParameterStatus::MESSAGE_CODE:
        msg = make_param_status_msg(payload, payload_len);
        break;
    case psql::Query::MESSAGE_CODE:
        msg = make_query_msg(payload, payload_len);
        break;
    case psql::Terminate::MESSAGE_CODE:
        msg = make_terminate_msg(payload, payload_len);
        break;
    default:
        break;
    }
    return msg;
}
