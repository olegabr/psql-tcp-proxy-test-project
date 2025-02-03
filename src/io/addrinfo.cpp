/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "addrinfo.hpp"
#include "v4.hpp"
#include "error.hpp"

#include <string>
#include <cstring> // std::memset
#include <utility> // std::exchange

io::ip::system::addrinfo_t::addrinfo_t(const io::ip::v4 &address)
{
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    const std::string port = std::to_string(address.port());
    int gai_error = ::getaddrinfo(address.host().c_str(), port.c_str(), &hints, &_addrinfo);
    if (gai_error != 0)
    {
        throw io::error(::gai_strerror(gai_error), -1, errno);
    }
}

io::ip::system::addrinfo_t::~addrinfo_t() noexcept
{
    if (_addrinfo)
    {
        ::freeaddrinfo(_addrinfo);
    }
}

void io::ip::system::swap(io::ip::system::addrinfo_t &lh, io::ip::system::addrinfo_t &rh)
{
    using std::swap;
    swap(lh._addrinfo, rh._addrinfo);
}

io::ip::system::addrinfo_t::addrinfo_t(io::ip::system::addrinfo_t &&other) noexcept
    : _addrinfo(std::exchange(other._addrinfo, nullptr))
{
}

io::ip::system::addrinfo_t &io::ip::system::addrinfo_t::operator=(io::ip::system::addrinfo_t &&other) noexcept
{
    io::ip::system::addrinfo_t temp{std::move(other)};
    swap(*this, temp);
    return *this;
}
