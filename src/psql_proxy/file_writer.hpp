/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_FILE_WRITER_T
#define H_PSQL_PROXY_FILE_WRITER_T

#include "data_processor.hpp"

#include <io/context.hpp>
#include <fstream>

/// @brief The PostgreSQL Proxy service namespace
namespace psql_proxy
{
    /// @brief The thread function object to dump SQL queries to a file
    class file_writer
    {
    public:
        /// @brief Construct the thread function object to dump SQL queries to a file
        /// @param io_context The I/O reactor pattern object. Uset to check for exit condition.
        /// @param data_processor The PostgreSQL messages processor object.
        /// @param ofs The file stream object to dump queries to.
        file_writer(const io::context_ptr &io_context, data_processor *data_processor, std::ofstream *ofs);

        /// @brief The thread body function
        void operator()();

    private:
        /// @brief The I/O reactor pattern object. Uset to check for exit condition.
        io::context_ptr _io_context;
        /// @brief The PostgreSQL messages processor object.
        data_processor *_data_processor;
        /// @brief The file stream object to dump queries to.
        std::ofstream *_ofs;
    };
}

#endif // H_PSQL_PROXY_FILE_WRITER_T
