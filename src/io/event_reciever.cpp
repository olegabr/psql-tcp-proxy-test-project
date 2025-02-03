/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "event_reciever.hpp"

void io::event_reciever::enqueue_event(file_descriptor_t fd, io::flags mask)
{
    _enqueue_event(fd, mask);
}

// LCOV_EXCL_START

io::event_reciever::~event_reciever() noexcept {}

// LCOV_EXCL_STOP
