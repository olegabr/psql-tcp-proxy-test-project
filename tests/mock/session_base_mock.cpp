/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "session_base_mock.hpp"

io::test::session_base_mock::session_base_mock(const io::bus_ptr &bus, io::file_descriptors_vec_t fds)
    : io::ip::tcp::session_base::session_base(bus, fds)
{
}

io::test::session_base_mock::~session_base_mock() noexcept
{
}