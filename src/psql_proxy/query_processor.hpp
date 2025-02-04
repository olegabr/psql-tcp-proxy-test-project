/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_QUERY_PROCESSOR_T
#define H_PSQL_PROXY_QUERY_PROCESSOR_T

#include "message_logger.hpp"
#include "data_processor.hpp"

#include <io/bipartite_buf.hpp>

#include <cstddef>
#include <string>
#include <functional>

/// @brief The PostgreSQL Proxy service namespace
namespace psql_proxy
{
    /// @brief The PostgreSQL messages processor object
    /// It's responsibility is:
    /// 1. collect messages using the \ref message_logger interface, and
    /// 2. provide access for the collected messages as a one concatenated string
    class query_processor final
        : public message_logger,
          public data_processor
    {
    public:
        /// @brief Construct the PostgreSQL messages processor object
        /// @param query_log The file stream object to dump queries to.
        /// @param separator The separator character for messages concatenation.
        explicit query_processor(char separator);
        ~query_processor() noexcept override;

    private:
        /// @brief Log the \p message string
        /// @param message The message string to log
        void
        _add_message(const std::string &message) override;
        /// @brief Flush the output buffer to the output stream
        /// @param callback The callback function to provide actual messages processing code
        /// @return The processed messages buffer length
        std::size_t _process(const data_processor::processor_callback_t &callback) override;

    private:
        /// @brief The buffer size chunk
        static constexpr std::size_t CHUNK_SZ = 1024;
        /// @brief The buffer size
        static constexpr std::size_t BUFF_SZ = 2 * CHUNK_SZ;
        /// @brief The buffer type to read to and write from
        using query_buffer_t = io::util::bipartite_buffer<char, BUFF_SZ, io::util::thread_safety_atomic>;

        /// @brief The buffer to read to and write from
        query_buffer_t _query_buffer;
        /// @brief The separator character for messages concatenation.
        char _separator;
    };
}

#endif // H_PSQL_PROXY_QUERY_PROCESSOR_T
