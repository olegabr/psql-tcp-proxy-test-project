/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "data_processor.hpp"

std::size_t psql_proxy::data_processor::process(const processor_callback_t &callback)
{
    return _process(callback);
}

// LCOV_EXCL_START
psql_proxy::data_processor::~data_processor()
{
}
// LCOV_EXCL_STOP
