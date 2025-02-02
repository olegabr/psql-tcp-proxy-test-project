/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "query.hpp"

#include <algorithm> // std::max
#include <iterator>  // std::next

psql::Query psql::make_query_msg(const void *payload, const std::size_t payload_len)
{
    psql::Query msg;

    const char *beg = static_cast<const char *>(payload);
    const char *end = std::next(beg, std::max(0ul, payload_len - 1));

    msg.query = std::string(beg, end);

    return msg;
}
