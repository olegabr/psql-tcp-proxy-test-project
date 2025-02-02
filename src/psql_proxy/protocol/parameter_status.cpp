/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "parameter_status.hpp"

#include <optional>

psql::ParameterStatus psql::make_param_status_msg(const void *payload, const std::size_t payload_len)
{
    psql::ParameterStatus msg;

    const char *params_beg = static_cast<const char *>(payload);
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
                psql::runtime_parameter param{*param_name, std::string(params_beg, params_cur)};
                msg.parameters.push_back(std::move(param));
                param_name.reset();
            }
            params_beg = params_cur + 1;
        }
    }

    return msg;
}
