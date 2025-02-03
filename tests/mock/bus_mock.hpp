/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_TEST_BUS_MOCK_T
#define H_IO_TEST_BUS_MOCK_T

#include <io/bus.hpp>

#include <cstddef>
#include <set>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The test specific namespace
    namespace test
    {
        /// \brief The \ref io::bus implementation based on the GNU/Linux kernel bus_mock async I/O API.
        class bus_mock
            : public io::bus
        {
        public:
            /// @brief Constructs new bus_mock \ref io::bus implementation object.
            explicit bus_mock();
            /// @brief Make sure the object is correctly destructed
            ~bus_mock() noexcept override;

            /// \brief copy is prohibited
            bus_mock(const bus_mock &) = delete; // non construction-copyable
            /// \brief copy is prohibited
            bus_mock &operator=(const bus_mock &) = delete; // non copyable

            /// \brief move is prohibited
            bus_mock(bus_mock &&) noexcept = delete;
            /// \brief move is prohibited
            bus_mock &operator=(bus_mock &&) noexcept = delete;

            void set_throw_io_error(bool do_throw);
            void set_throw_std_exception(bool do_throw);
            void set_throw_unknown_error(bool do_throw);

        private:
            /// @brief Listen on the \p fd file descriptor.
            /// The \ref io::bus::_add_fd pure virtual function implementation.
            /// @param fd The file descriptor
            void _add_fd(file_descriptor_t fd) override;
            /// @brief Stop listening on the \p fd file descriptor
            /// The \ref io::bus::_del_fd pure virtual function implementation.
            /// @param fd The file descriptor
            void _del_fd(file_descriptor_t fd) override;

            /// @brief Wait for I/O events on this bus object.
            /// It waits for the \p timeout_msec milliseconds or
            /// while the \p events_buf_size events is read.
            /// The \ref io::bus::_wait_events pure virtual function implementation.
            /// @param timeout_msec The maximum time to wait for events, in milliseconds. Or \ref std::chrono::milliseconds{0} for infinit wait.
            /// @param events_buf_size The events buffer size
            /// @param callback The I/O bus async event callback function to be called for each I/O event triggered.
            void _wait_events(std::chrono::milliseconds timeout_msec, std::size_t events_buf_size, io::bus::callback_t callback) override;

        private:
            /// \brief The native bus_mock events buffer type.
            using file_descriptors_set_t = std::set<file_descriptor_t>;
            /// \brief The native bus_mock events buffer.
            file_descriptors_set_t _file_descriptors;

            bool _do_throw_io_error;
            bool _do_throw_std_exception;
            bool _do_throw_unknown_error;
        };
    }
}

#endif // H_IO_TEST_BUS_MOCK_T
