/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_QUERY_PROCESSOR_T
#define H_PSQL_PROXY_QUERY_PROCESSOR_T

#include <io/bipartite_buf.hpp>

#include <cstddef>
#include <string>
#include <fstream>

/// @brief The PostgreSQL Proxy service namespace
namespace psql_proxy
{
    /// @brief The PostgreSQL messages interpreter object
    class query_processor
    {
    public:
        /// @brief Construct the PostgreSQL messages interpreter object
        /// @param query_log The file stream object to dump queries to.
        explicit query_processor(std::ofstream &&query_log);

        /// @brief Add the \p query string to the output buffer
        /// @param query The SQL query string
        void add_query(const std::string &query);
        /// @brief Flush the output buffer to the output stream
        /// @return The number of characters written
        int process();
        /// @brief Flush the output stream
        void flush();

    private:
        /// @brief The buffer size chunk
        static constexpr std::size_t CHUNK_SZ = 1024;
        /// @brief The buffer size
        static constexpr std::size_t BUFF_SZ = 2 * CHUNK_SZ;
        /// @brief The buffer type to read to and write from
        using query_buffer_t = io::util::bipartite_buffer<char, BUFF_SZ, io::util::thread_safety_atomic>;

        /// @brief The buffer to read to and write from
        query_buffer_t _query_buffer;
        /// @brief The file stream object to dump queries to.
        std::ofstream _query_log;
    };
}

#endif // H_PSQL_PROXY_QUERY_PROCESSOR_T
