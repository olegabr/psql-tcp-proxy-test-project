/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_HANDLER_T
#define H_PSQL_PROXY_HANDLER_T

#include "message.hpp"
#include "message_logger.hpp"

#include <io/fd.hpp>
#include <io/object.hpp>
#include <io/bus.hpp>

#include <cstddef>
#include <vector>

/// @brief The PostgreSQL Proxy service namespace
namespace psql_proxy
{
    /// @brief The \ref io::input_object::callback_t callback
    class handler
    {
    public:
        /// @brief Construct the \ref io::input_object::callback_t callback
        /// @param logger The \ref message_logger object to interpret PostgreSQL messages
        /// @param fd The file descriptor of the client connection socket to report disconnect message to
        /// @param bus The \ref io::bus object pointer to report disconnect message to
        handler(
            message_logger *logger,
            io::file_descriptor_t fd,
            io::bus *bus);

        /// @brief The I/O operation result callback.
        /// @sa \ref io::input_object::callback_t
        void operator()(const io::input_object::result_type &result);

    private:
        /// @brief The temporary buffer for data processing
        std::vector<std::byte> _buffer;
        /// @brief True if the first message was already handled
        bool _first_msg_handled;
        /// @brief The \ref message_logger object to interpret PostgreSQL messages
        message_logger *_message_logger;
        /// @brief The file descriptor of the client connection socket to report disconnect message to
        io::file_descriptor_t _fd;
        /// @brief The \ref io::bus object pointer to report disconnect message to
        io::bus *_bus;
    };
}

#endif // H_PSQL_PROXY_HANDLER_T
