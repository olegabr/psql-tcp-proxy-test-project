/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "message_logger.hpp"

void psql_proxy::message_logger::add_message(const std::string &message)
{
    _add_message(message);
}

// LCOV_EXCL_START
psql_proxy::message_logger::~message_logger()
{
}
// LCOV_EXCL_STOP
