/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "bus.hpp"
#include "log.hpp"

#include <algorithm>
#include <iostream>

// LCOV_EXCL_START
io::bus::~bus()
{
    IO_DEBUG((std::cout << "~bus" << std::endl));
}
// LCOV_EXCL_STOP

void io::bus::add_fd(file_descriptor_t fd, callback_t callback)
{
    auto i = _callback_map.find(fd);
    if (_callback_map.end() == i)
    {
        _callback_map.insert(std::make_pair(fd, std::vector<callback_t>()));
        i = _callback_map.find(fd);
        _add_fd(fd);
    }
    if (callback)
    {
        i->second.push_back(callback);
    }
}

void io::bus::del_fd_callbacks(file_descriptor_t fd)
{
    _callbacks_to_remove.insert(fd);
}

void io::bus::del_fd(file_descriptor_t fd)
{
    _callbacks_to_remove.insert(fd);
    _del_fd(fd);
}

void io::bus::wait_events(int timeout_msec, std::size_t events_buf_size, io::bus::error_callback_t error_callback)
{
    auto cb = [this, error_callback](event_reciever *reciever, file_descriptor_t fd, flags mask)
    {
        auto cb = [this, fd, mask, error_callback, reciever](callback_t callback)
        {
            try
            {
                if (_callbacks_to_remove.end() == _callbacks_to_remove.find(fd))
                {
                    callback(reciever, fd, mask);
                }
            }
            catch (io::error &ex)
            {
                error_callback(reciever, ex);
            }
            catch (std::exception &ex)
            {
                error_callback(reciever, io::error(ex.what(), fd, errno));
            }
            catch (...)
            {
                error_callback(reciever, io::error("unknown io error", fd, errno));
            }
        };

        auto i = _callback_map.find(fd);
        if (_callback_map.end() != i)
        {
            std::for_each(i->second.begin(), i->second.end(), cb);
        }

        // remove callbacks for closed connections
        auto beg = _callbacks_to_remove.begin();
        auto end = _callbacks_to_remove.end();
        std::for_each(
            beg,
            end,
            [this](io::file_descriptor_t fd_to_remove)
            {
                _callback_map.erase(fd_to_remove);
            });
        _callbacks_to_remove.clear();
    };
    try
    {
        _wait_events(timeout_msec, events_buf_size, cb);
        // prevent infinite events generation loop
        events_queue_t events;
        std::swap(_events, events);
        while (!events.empty())
        {
            const auto &event = events.front();
            cb(this, event.fd, event.flags);
            events.pop();
        }
    }
    catch (io::error &ex)
    {
        error_callback(this, ex);
    }
    catch (std::exception &ex)
    {
        error_callback(this, io::error(ex.what(), -1, errno));
    }
    catch (...)
    {
        error_callback(this, io::error("unknown io error", -1, errno));
    }
}

void io::bus::_enqueue_event(file_descriptor_t fd, io::flags f)
{
    _events.push(event_t{fd, f});
}