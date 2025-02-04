/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "query_processor.hpp"

psql_proxy::query_processor::query_processor(char separator)
    : _separator(separator)
{
}

psql_proxy::query_processor::~query_processor() noexcept
{
}

void psql_proxy::query_processor::_add_message(const std::string &message)
{
    auto pos = message.begin();
    auto end = message.end();
    do
    {
        auto last = end;
        if (CHUNK_SZ < std::distance(pos, end))
        {
            last = std::next(pos + CHUNK_SZ);
        }
        const std::size_t length = std::distance(pos, last);
        auto wbuf = _query_buffer.write_acquire(length);
        if (nullptr != wbuf)
        {
            std::copy(pos, last, wbuf);
            _query_buffer.write_release(length);
        }
        pos = last;
    } while (pos != end);

    auto wbuf = _query_buffer.write_acquire(1);
    if (nullptr != wbuf)
    {
        *wbuf = _separator;
        _query_buffer.write_release(1);
    }
}

std::size_t psql_proxy::query_processor::_process(const data_processor::processor_callback_t &callback)
{
    auto [rbuf, len] = _query_buffer.read_acquire();
    if (nullptr != rbuf && len > 0)
    {
        auto written_chars = callback(rbuf, len);
        _query_buffer.read_release(written_chars);
        return written_chars;
    }
    return 0;
}
