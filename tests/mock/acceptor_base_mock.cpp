/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "acceptor_base_mock.hpp"

io::test::acceptor_base_mock::acceptor_base_mock(
    io::bus_ptr io_bus,
    callback_t callback)
    : io::ip::acceptor_base(io_bus, 1)
{
    add_callback(callback);
}

io::test::acceptor_base_mock::~acceptor_base_mock()
{
}

std::pair<io::file_descriptor_t, io::ip::v4> io::test::acceptor_base_mock::_accept_new_client(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask)
{
    io::ip::v4 address{"127.0.0.1", "12345"};
    return std::make_pair(1, address);
}
