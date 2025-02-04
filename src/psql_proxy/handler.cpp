/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "handler.hpp"
#include "endianness.hpp"

#include "io/bipartite_buf.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <type_traits>

psql_proxy::handler::handler(
    message_logger *logger,
    io::file_descriptor_t fd,
    io::bus *bus)
    : _first_msg_handled(false),
      _message_logger(logger),
      _fd(fd),
      _bus(bus)
{
}

namespace
{
    struct PSQL_Message_Visitor
    {
        PSQL_Message_Visitor(
            psql_proxy::message_logger *logger,
            io::file_descriptor_t fd,
            io::bus *bus)
            : _message_logger(logger),
              _fd(fd),
              _bus(bus)
        {
        }

        void operator()(const psql::StartupMessage &m) const
        {
            std::cout << "protocol version " << m.protocol_version.major << '.' << m.protocol_version.minor << '\n';
        }
        void operator()(const psql::Query &m)
        {
            // std::cout << m.query << '\n';
            _message_logger->add_message(m.query);
        }
        void operator()(const psql::Terminate &m)
        {
            std::cout << "Terminate message recieved" << '\n';
            errno = 0;
            _bus->enqueue_event(_fd, io::flags::error);
        }
        void operator()(const psql::ParameterStatus &m) const
        {
            std::cout << "ParameterStatus" << '\n';
        }

    private:
        psql_proxy::message_logger *_message_logger;
        io::file_descriptor_t _fd;
        io::bus *_bus;
    };
}
// @see http://www.prog.org.ru/index.php?topic=33504.msg247481#msg247481
// uint8_t type;
// uint32_t length;
// uint8_t payload[length - sizeof(length)];
void psql_proxy::handler::operator()(const io::input_object::result_type &result)
{
    auto v = io::make_visitor{
        [](const io::error &err)
        {
            ; // ignore errors
        },
        [&](const io::input_object::success_result_type &res)
        {
            // TODO: use std::ranges library from c++20
            // to merge these two branches and simplify code logic

            // avoid copying whenever possible
            if (_buffer.empty())
            {
                auto *pos_orig = static_cast<const std::byte *>(res.buf);
                auto *pos = pos_orig;
                auto *end = std::next(pos, res.buf_len);
                while (0ul < std::distance(pos, end))
                {
                    std::byte msg_code{'\0'};
                    if (_first_msg_handled)
                    {
                        msg_code = *pos;
                        pos = std::next(pos);
                    }

                    if (std::distance(pos, end) < sizeof(uint32_t))
                    {
                        // need to read more data to process
                        std::cout
                            << "need to read more data to process" << std::endl;
                        _buffer.resize(std::distance(pos_orig, end));
                        std::copy(pos_orig, end, _buffer.begin());
                        break;
                    }
                    io::endianness endianness = psql::check_endianness_by_uint32(pos);
                    const uint32_t payload_length_with_length = io::decode_uint32(pos, endianness);
                    if (payload_length_with_length < sizeof(uint32_t))
                    {
                        std::cout
                            << "empty payload: payload_length_with_length = " << payload_length_with_length << "; packet type: " << std::hex << (0xFF & std::to_integer<int>(msg_code)) << std::dec << '(' << std::to_integer<char>(msg_code) << ')' << std::endl;
                        _buffer.resize(std::distance(pos_orig, end));
                        std::copy(pos_orig, end, _buffer.begin());
                        break;
                    }
                    const uint32_t payload_length = payload_length_with_length - sizeof(uint32_t);

                    pos = std::next(pos, sizeof(uint32_t));
                    if (std::distance(pos, end) < payload_length)
                    {
                        // need to read more data to process
                        std::cout << "fd = " << res.fd << "; " << "not enough data to read payload: " << std::distance(pos, end) << " < " << payload_length << std::endl;
                        _buffer.resize(std::distance(pos_orig, end));
                        std::copy(pos_orig, end, _buffer.begin());
                        break;
                    }
                    std::optional<psql::message> msg = psql::make_message(msg_code, pos, payload_length, endianness);
                    _first_msg_handled = true;
                    pos = std::next(pos, payload_length);
                    pos_orig = pos;
                    if (msg)
                    {
                        std::visit(PSQL_Message_Visitor(_message_logger, _fd, _bus), *msg);
                    }
                };
            }
            else
            {
                // this code is not used normally

                const auto prev_sz = _buffer.size();
                _buffer.resize(_buffer.size() + res.buf_len);
                auto bytes = static_cast<const std::byte *>(res.buf);
                std::copy(bytes, std::next(bytes, res.buf_len), std::next(_buffer.begin(), prev_sz));

                bool do_continue = false;
                do
                {
                    do_continue = false;
                    const std::size_t sz = _buffer.size();
                    if (0 == sz)
                    {
                        break;
                    }
                    auto *pos = _buffer.data();
                    auto *end = std::next(pos, sz);

                    std::byte msg_code{'\0'};
                    if (_first_msg_handled)
                    {
                        msg_code = *pos;
                        pos = std::next(pos);
                    }

                    if (std::distance(pos, end) < sizeof(uint32_t))
                    {
                        // need to read more data to process
                        std::cout << "fd = " << res.fd << "; " << "not enough data to read payload length\n";
                        break;
                    }
                    io::endianness endianness = psql::check_endianness_by_uint32(pos);
                    const uint32_t payload_length_with_length = io::decode_uint32(pos, endianness);
                    if (payload_length_with_length < sizeof(uint32_t))
                    {
                        std::cout
                            << "empty payload: payload_length_with_length = " << payload_length_with_length << "; packet type: " << std::hex << (0xFF & std::to_integer<int>(msg_code)) << std::dec << '(' << std::to_integer<char>(msg_code) << ')' << std::endl;
                        break;
                    }
                    const uint32_t payload_length = payload_length_with_length - sizeof(uint32_t);

                    pos = std::next(pos, sizeof(uint32_t));
                    if (std::distance(pos, end) < payload_length)
                    {
                        // need to read more data to process
                        // std::cout << "fd = " << res.fd << "; " << "not enough data to read payload: " << std::distance(pos, end) << " < " << payload_length << std::endl;
                        break;
                    }
                    std::optional<psql::message> msg = psql::make_message(msg_code, pos, payload_length, endianness);
                    _first_msg_handled = true;
                    pos = std::next(pos, payload_length);
                    if (std::distance(pos, end) > 0)
                    {
                        std::vector<std::byte> new_buffer(pos, end);
                        _buffer.swap(new_buffer);
                        do_continue = true;
                    }
                    else
                    {
                        _buffer.clear();
                    }
                    if (msg)
                    {
                        std::visit(PSQL_Message_Visitor(_message_logger, _fd, _bus), *msg);
                    }
                } while (do_continue);
            }
        }};
    std::visit(v, result);
}
