/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_EVENT_RECIEVER_T
#define H_IO_EVENT_RECIEVER_T

#include "fd.hpp"
#include "flags.hpp"

#include <memory>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The async I/O event_reciever abstraction.
    /// It is an abstract class which can not be instantiated.
    /// Inherit from it to implement the real I/O event_reciever.
    class event_reciever
    {
    public:
        /// @brief Enqueue event on the \p fd file descriptor with the
        /// \p mask event mask value
        /// @param fd The file descriptor
        /// @param mask The I/O event mask
        void enqueue_event(file_descriptor_t fd, io::flags mask);

    protected:
        /// @brief The event_reciever destructor
        virtual ~event_reciever() noexcept;

    private:
        /// @brief Enqueue event on the \p fd file descriptor with the
        /// \p mask event mask value
        /// @param fd The file descriptor
        /// @param mask The I/O event mask
        virtual void _enqueue_event(file_descriptor_t fd, io::flags mask) = 0;
    };
    /// \brief The async I/O event_reciever abstraction smart pointer.
    using event_reciever_ptr = std::shared_ptr<event_reciever>;
}

#endif // H_IO_EVENT_RECIEVER_T
