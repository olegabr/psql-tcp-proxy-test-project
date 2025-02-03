/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "acceptor_base.hpp"
#include "v4.hpp"
#include "error.hpp"

#include <algorithm>
#include <iostream>

io::ip::acceptor_base::acceptor_base(const io::bus_ptr &io_bus, io::file_descriptor_t fd)
    : _io_bus(io_bus),
      _fd(fd)
{
    add_bus_callback(
        [this](io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
        {
            auto res = _accept_new_client(reciever, fd, mask);
            io::file_descriptor_t conn = res.first;
            if (0 < conn)
            {
                const io::ip::v4 &address = res.second;
                for (const auto &callback : _callbacks)
                {
                    callback(conn, address);
                }
            }
        });
}

io::file_descriptor_t io::ip::acceptor_base::_get_fd() const
{
    return _fd;
}

const io::bus_ptr &io::ip::acceptor_base::_get_bus()
{
    return _io_bus;
}

// LCOV_EXCL_START
io::ip::acceptor_base::~acceptor_base() noexcept
{
    try
    {
        _close();
    }
    catch (io::error &ex)
    {
        std::cerr << ex.what() << "; errno = " << ex.get_errno() << "; for fd = " << ex.get_fd() << '\n';
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
// LCOV_EXCL_STOP

void io::ip::acceptor_base::_close()
{
    if (-1 != _fd)
    {
        _io_bus->del_fd(_fd);
        _fd = -1;
    }
}

void io::ip::acceptor_base::add_callback(callback_t callback)
{
    if (callback)
    {
        _callbacks.push_back(std::move(callback));
    }
}
