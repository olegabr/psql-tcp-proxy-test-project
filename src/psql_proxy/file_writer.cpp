/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "file_writer.hpp"

#include <chrono>
#include <thread>

psql_proxy::file_writer::file_writer(
    const io::context_ptr &io_context,
    data_processor *data_processor,
    std::ofstream *ofs)
    : _io_context(io_context),
      _data_processor(data_processor),
      _ofs(ofs)
{
}

void psql_proxy::file_writer::operator()()
{
    using timer = std::chrono::system_clock;
    auto tm = timer::now();
    while (!_io_context->is_stop_requested())
    {
        // std::cout << "writer thread" << std::endl;
        const int written_chars = _data_processor->process(
            [&](const char *buf, std::size_t buf_len) -> std::size_t
            {
                auto written_chars = _ofs->rdbuf()->sputn(buf, buf_len);
                return written_chars;
            });
        if (0 == written_chars)
        {
            // An alternative is a conditional with a mutex, but it would add significant overhead
            // while this task can safely wait for 100 msec if no data available.
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        else
        {
            // std::cout << "writer thread written_chars = " << written_chars << std::endl;
        }
        auto now = timer::now();
        auto elapsed = now - tm;
        if (100 <= std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count())
        {
            _ofs->flush();
            tm = now;
        }
    }
    _ofs->flush();
}