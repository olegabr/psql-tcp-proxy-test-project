/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "session_base.hpp"
#include <algorithm>
#include <iostream>
#include "log.hpp"

io::ip::tcp::session_base::session_base(const io::bus_ptr &bus, io::file_descriptors_vec_t fds)
    : _bus(bus),
      _fds(fds)
{
}

void io::ip::tcp::session_base::start()
{
    auto self(shared_from_this());
    auto cb = [this, self](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
    {
        if (mask.test(io::flags::error))
        {
            for (io::file_descriptor_t fd : _fds)
            {
                _bus->del_fd_callbacks(fd);
            }
        }
    };
    for (io::file_descriptor_t fd : _fds)
    {
        _bus->add_fd(fd, cb);
    }
}

// LCOV_EXCL_START
io::ip::tcp::session_base::~session_base() noexcept
{
}
// LCOV_EXCL_STOP
