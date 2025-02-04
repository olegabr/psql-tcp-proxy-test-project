/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_MESSAGE_LOGGER_T
#define H_PSQL_PROXY_MESSAGE_LOGGER_T

#include <string>

/// @brief The PostgreSQL Proxy service namespace
namespace psql_proxy
{
    /// @brief The PostgreSQL messages interpreter object
    class message_logger
    {
    public:
        /// @brief Log the \p message string
        /// @param message The message string to log
        void add_message(const std::string &message);

    protected:
        /// @brief Destruct the PostgreSQL message logger object
        virtual ~message_logger();

    private:
        /// @brief Log the \p message string
        /// @param message The message string to log
        virtual void _add_message(const std::string &message) = 0;
    };
}

#endif // H_PSQL_PROXY_MESSAGE_LOGGER_T
