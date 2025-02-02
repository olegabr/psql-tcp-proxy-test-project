/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_SOCKET_PIPE_T
#define H_SOCKET_PIPE_T

#include "object.hpp"
#include "bus.hpp"
#include "bipartite_buf.hpp"

#include <cstddef>
#include <memory>

/// \brief The input/output library namespace
namespace io
{
    class channel;
    /// \brief The I/O channel/pipe/tube pattern implementation smart pointer.
    using channel_ptr = std::shared_ptr<channel>;
    /// \brief Construct the I/O channel/pipe/tube pattern implementation object.
    /// @param left Input object to read data from
    /// @param right Output object to write data to
    channel_ptr make_channel(
        const io::input_object_ptr &left,
        const io::output_object_ptr &right);

    /// \brief The I/O channel/pipe/tube pattern implementation.
    /// It reads data from \ref io::input_object and writes it to
    /// the \ref io::output_object provided in constructor.
    class channel
        : public std::enable_shared_from_this<channel>
    {
    public:
        /// @brief The I/O operation result callback function type.
        /// The file descriptor,
        /// data buffer pointer and bytes recieved or sent count
        /// are provided for the callback when called
        using input_callback_t = io::input_object::callback_t;

        /// \brief Add an input object handler.
        /// The chain of responsibility pattern can be implemented with this function.
        /// @param cb The \ref io::input_object callback to check or modify
        /// data before write it to \ref io::output_object
        void add_handler(input_callback_t &&cb);

        /// @brief Get the input object to read data from
        /// @return The input object to read data from
        io::input_object_ptr get_left()
        {
            return _left;
        }
        /// @brief Get the output object to write data to
        /// @return The output object to write data to
        io::output_object_ptr get_right()
        {
            return _right;
        }

        ~channel();

    private:
        /// \brief Construct the I/O channel/pipe/tube pattern implementation object.
        /// @param left Input object to read data from
        /// @param right Output object to write data to
        channel(
            const io::input_object_ptr &left,
            const io::output_object_ptr &right);

        /// \brief Initialize bus callbacks for input and output objects and
        /// implements the enable from this idiom.
        void _start();
        friend channel_ptr make_channel(
            const io::input_object_ptr &left,
            const io::output_object_ptr &right);

        /// \brief The input object I/O bus async event callback handler function
        /// @param reciever The async I/O event_reciever abstraction
        /// @param fd The file descriptor
        /// @param mask The I/O event mask
        void _handle_left_io_event(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask);
        /// \brief Constuct the input object I/O bus async event callback
        /// @return The input object I/O bus async event callback
        io::bus::callback_t _make_left_socket_callback();

        /// \brief The output object I/O bus async event callback handler function
        /// @param reciever The async I/O event_reciever abstraction
        /// @param fd The file descriptor
        /// @param mask The I/O event mask
        void _handle_right_io_event(io::event_reciever *reciever, io::file_descriptor_t fd, io::flags mask);
        /// \brief Constuct the output object I/O bus async event callback
        /// @return The output object I/O bus async event callback
        io::bus::callback_t _make_right_socket_callback();

        /// @brief The buffer size chunk
        static constexpr std::size_t CHUNK_SZ = 64 * 1024 - 1;
        /// @brief The buffer size
        static constexpr std::size_t BUFF_SZ = 2 * CHUNK_SZ;
        /// @brief The buffer type to read to and write from
        using buffer_t = io::util::bipartite_buffer<std::byte, BUFF_SZ, io::util::thread_safety_noop>;
        /// @brief The buffer to read to and write from
        buffer_t _buffer;

        /// @brief Input object to read data from
        io::input_object_ptr _left;
        /// @brief Output object to write data to
        io::output_object_ptr _right;

        /// @brief The I/O operation result callbacks
        std::vector<input_callback_t> _handlers;
    };
}
#endif // H_SOCKET_PIPE_T
