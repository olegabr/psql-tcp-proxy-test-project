/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "startup_message.hpp"

#include <stdexcept>
#include <optional>

psql::StartupMessage psql::make_startup_msg(const void *payload, const std::size_t payload_len, io::endianness endianness)
{
    psql::StartupMessage msg;

    if (payload_len < sizeof(uint32_t))
    {
        throw std::runtime_error("startup message bad length");
    }

    msg.protocol_version.major = io::decode_uint16(static_cast<const std::byte *>(payload), endianness);
    msg.protocol_version.minor = io::decode_uint16(static_cast<const std::byte *>(payload) + sizeof(uint16_t), endianness);
    const char *params_beg = static_cast<const char *>(payload) + sizeof(uint32_t);
    const char *params_cur = params_beg;
    const char *params_end = params_beg + payload_len;

    std::optional<std::string> param_name;
    for (; params_end != params_cur; ++params_cur)
    {
        if ('\0' == *params_cur)
        {
            if (!param_name)
            {
                param_name = std::string(params_beg, params_cur);
            }
            else
            {
                psql::configuration_parameter param{*param_name, std::string(params_beg, params_cur)};
                msg.parameters.push_back(std::move(param));
                param_name.reset();
            }
            params_beg = params_cur + 1;
        }
    }

    return msg;
}
