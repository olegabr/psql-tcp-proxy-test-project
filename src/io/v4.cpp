/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "v4.hpp"

#include <string>
#include <array>
#include <cstring>
#include <ostream>

#include <arpa/inet.h>

io::ip::v4::v4(std::string const &host, std::string const &port)
    : _host(host), _port(std::stoi(port))
{
}

void io::ip::swap(io::ip::v4 &&lh, io::ip::v4 &&rh)
{
    using std::swap;
    swap(lh._host, rh._host);
    swap(lh._port, rh._port);
}

io::ip::v4::v4(io::ip::v4 &&other) noexcept
    : _host(std::move(other._host)), _port(std::exchange(other._port, 0))
{
}

io::ip::v4 &io::ip::v4::operator=(io::ip::v4 &&other) noexcept
{
    if (&other == this)
    {
        // self assign protection
        return *this;
    }
    io::ip::v4 temp{std::move(other)};
    using std::swap;
    swap(std::move(*this), std::move(temp));
    return *this;
}

std::ostream &io::ip::operator<<(std::ostream &stream, const io::ip::v4 &address)
{
    stream << address._host << ':' << address._port;
    return stream;
}
