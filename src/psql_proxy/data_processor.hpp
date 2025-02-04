/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_PSQL_PROXY_DATA_PROCESSOR_T
#define H_PSQL_PROXY_DATA_PROCESSOR_T

#include <cstddef>
#include <functional>

/// @brief The PostgreSQL Proxy service namespace
namespace psql_proxy
{
    /// @brief The data buffer processing abstraction
    class data_processor
    {
    public:
        /// @brief The callback function to provide actual processing code
        /// @param buf The available data buffer begin pointer
        /// @param buf_len The available data buffer length
        /// @returns The processed data buffer length
        using processor_callback_t = std::function<std::size_t(const char *buf, std::size_t buf_len)>;

        /// @brief Process data provided to the callback
        /// @param callback The callback function to provide actual processing code
        /// @return The processed data buffer length
        std::size_t process(const processor_callback_t &callback);

    protected:
        /// @brief Destruct the PostgreSQL message logger object
        virtual ~data_processor();

    private:
        /// @brief Process data provided to the callback
        /// @param callback The callback function to provide actual processing code
        /// @return The processed data buffer length
        virtual std::size_t _process(const processor_callback_t &callback) = 0;
    };
}

#endif // H_PSQL_PROXY_DATA_PROCESSOR_T
