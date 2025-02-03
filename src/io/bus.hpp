/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_BUS_T
#define H_IO_BUS_T

#include "fd.hpp"
#include "flags.hpp"
#include "error.hpp"
#include "event_reciever.hpp"

#include <functional>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <memory>
#include <queue>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The async I/O bus abstraction.
    /// It is an abstract class which can not be instantiated.
    /// Inherit from it to implement the real I/O bus.
    class bus
        : public event_reciever
    {
    public:
        /// \brief The I/O bus async event callback type.
        /// The callback accepts the \ref io::event_reciever* event reciever pointer,
        /// \ref io::file_descriptor_t file descriptor and
        /// the \ref io::flags flags parameters
        using callback_t = std::function<void(event_reciever *, file_descriptor_t, io::flags)>;
        /// \brief The I/O bus async error callback type
        /// The callback accepts the \ref io::event_reciever* event reciever pointer and
        /// the \ref io::error error object as parameter
        using error_callback_t = std::function<void(event_reciever *, const io::error &)>;

        /// @brief Listen on the \p fd file descriptor and call the
        /// \p callback function on each bus event detected for this \p fd
        /// @param fd The file descriptor
        /// @param callback The bus event handler to call back user code
        void add_fd(file_descriptor_t fd, callback_t callback = nullptr);
        /// @brief Remove all callbacks registered for the \p fd file descriptor
        /// @param fd The file descriptor
        void del_fd_callbacks(file_descriptor_t fd);
        /// @brief Stop listening on the \p fd file descriptor
        /// @param fd The file descriptor
        void del_fd(file_descriptor_t fd);

        /// @brief Wait for I/O events on this bus object.
        /// It waits for the \p timeout_msec milliseconds or
        /// while the \p events_buf_size events is read.
        /// @param timeout_msec The maximum time to wait for events, in milliseconds. Or -1 for infinit wait.
        /// @param events_buf_size The events buffer size
        /// @param error_callback The function to be called in a case of an I/O error occured.
        void wait_events(int timeout_msec, std::size_t events_buf_size, error_callback_t error_callback = nullptr);

    protected:
        /// @brief The bus destructor
        ~bus() noexcept override;

    private:
        /// @brief Listen on the \p fd file descriptor.
        /// Pure virtual function. Implement it in the inherited concrete bus class.
        /// @param fd The file descriptor
        virtual void _add_fd(file_descriptor_t fd) = 0;
        /// @brief Stop listening on the \p fd file descriptor.
        /// Pure virtual function. Implement it in the inherited concrete bus class.
        /// @param fd The file descriptor
        virtual void _del_fd(file_descriptor_t fd) = 0;
        /// @brief Wait for I/O events on this bus object.
        /// It waits for the \p timeout_msec milliseconds or
        /// while the \p events_buf_size events is read.
        /// Pure virtual function. Implement it in the inherited concrete bus class.
        /// @param timeout_msec The maximum time to wait for events, in milliseconds. Or -1 for infinit wait.
        /// @param events_buf_size The events buffer size
        /// @param callback The I/O bus async event callback function to be called for each I/O event triggered.
        virtual void _wait_events(int timeout_msec, std::size_t events_buf_size, callback_t callback) = 0;

    private:
        /// @brief Enqueue event on the \p fd file descriptor with the
        /// \p mask event mask value
        /// @param fd The file descriptor
        /// @param mask The I/O event mask
        void _enqueue_event(file_descriptor_t fd, io::flags mask) override;

    private:
        /// @brief The I/O callbacks container type for fast file descriptor to callbacks lookup
        using callback_map_t = std::unordered_map<file_descriptor_t, std::vector<callback_t>>;
        /// @brief The I/O callbacks container for fast file descriptor to callbacks lookup
        callback_map_t _callback_map;
        /// @brief Auxiliary file descriptors container to defer actual file descriptor deletion
        /// until the currently executing callback is finished.
        /// It is necessary to prevent deletion from the io::bus::_callback_map while iterating it.
        std::unordered_set<file_descriptor_t> _callbacks_to_remove;

        struct event_t
        {
            file_descriptor_t fd;
            io::flags flags;
        };
        using events_queue_t = std::queue<event_t>;
        events_queue_t _events;
    };
    /// \brief The async I/O bus abstraction smart pointer.
    using bus_ptr = std::shared_ptr<bus>;
}

#endif // H_IO_BUS_T
