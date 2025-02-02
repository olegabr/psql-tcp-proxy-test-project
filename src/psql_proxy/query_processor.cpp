/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "query_processor.hpp"

psql_proxy::query_processor::query_processor(std::ofstream &&query_log)
    : _query_log(std::move(query_log)) {}

void psql_proxy::query_processor::add_query(const std::string &query)
{
    auto pos = query.begin();
    auto end = query.end();
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
        *wbuf = '\n';
        _query_buffer.write_release(1);
    }
}

int psql_proxy::query_processor::process()
{
    auto [rbuf, len] = _query_buffer.read_acquire();
    if (nullptr != rbuf && len > 0)
    {
        auto written_chars = _query_log.rdbuf()->sputn(rbuf, len);
        _query_buffer.read_release(written_chars);
        return written_chars;
    }
    return 0;
}

void psql_proxy::query_processor::flush()
{
    _query_log.flush();
}