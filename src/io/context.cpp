/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "context.hpp"

io::context::context(
    const io::bus_ptr &io_bus,
    int timeout_ms,
    std::size_t min_buf_size)
    : _io_bus(io_bus),
      _stop_requested(false),
      _timeout_msec(timeout_ms),
      _events_buf_size(min_buf_size)
{
}

void io::context::stop()
{
    _stop_requested = true;
}

void io::context::run(io::bus::error_callback_t error_callback)
{
    while (!_stop_requested)
    {
        _io_bus->wait_events(_timeout_msec, _events_buf_size, error_callback);
    }
}
