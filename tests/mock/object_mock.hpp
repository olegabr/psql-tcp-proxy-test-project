/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_TEST_OBJECT_MOCK_T
#define H_IO_TEST_OBJECT_MOCK_T

#include <io/object.hpp>
#include <vector>
#include <cstddef> // std::byte

/// \brief The input/output library namespace
namespace io
{
    /// \brief The test specific namespace
    namespace test
    {
        /// \brief The async I/O object abstraction common functionality.
        /// This class is abstract and internal. Should not be used by client code.
        class object_base_mock
            : public virtual io::object_base
        {
        public:
            object_base_mock(io::bus_ptr bus, io::file_descriptor_t fd);
            /// @brief Make sure the object is correctly destructed
            ~object_base_mock() override;

            void set_throw_error(bool val);
            void set_result_buf_len(int val);
            void set_data_buffer(std::vector<std::byte> *buf);

        private:
            /// @brief Get the underlying file descriptor value for this I/O object
            /// Pure virtual function. Implement it in the inherited concrete I/O object class.
            /// @return The underlying file descriptor value for this I/O object
            file_descriptor_t _get_fd() const override;
            /// @brief Get the underlying \ref io::bus object for this I/O object
            /// Pure virtual function. Implement it in the inherited concrete I/O object class.
            /// @return The underlying \ref io::bus object for this I/O object
            const bus_ptr &_get_bus() override;

        protected:
            bool _throw_error;
            int _result_buf_len;
            std::vector<std::byte> *_buffer;

        private:
            io::bus_ptr _bus;
            io::file_descriptor_t _fd;
        };

        /// \brief The async input object abstraction.
        class input_object_mock
            : public virtual input_object,
              public virtual object_base_mock
        {
        public:
            input_object_mock(io::bus_ptr bus, io::file_descriptor_t fd);
            /// @brief Make sure the object is correctly destructed
            ~input_object_mock() override;

        private:
            /// @brief Read available data asynchronously.
            /// Pure virtual function. Implement it in the inherited concrete I/O object class.
            /// @param buf The buffer to write the recieved data to
            /// @param buf_len The length of the \p buf
            /// @return The \ref result_type with length of data read or error occured
            result_type _async_read_some(value_t *buf, std::size_t buf_len) override;
        };

        /// \brief The async output object abstraction.
        class output_object_mock
            : public virtual output_object,
              public virtual object_base_mock
        {
        public:
            output_object_mock(io::bus_ptr bus, io::file_descriptor_t fd);
            /// @brief Make sure the object is correctly destructed
            ~output_object_mock() override;

        private:
            /// @brief Write data asynchronously.
            /// Pure virtual function. Implement it in the inherited concrete I/O object class.
            /// @param buf The buffer of data to write from
            /// @param buf_len The length of the \p buf
            /// @return The \ref result_type with length of data written or error occured
            result_type _async_write_some(const value_t *buf, std::size_t buf_len) override;
        };
        /// \brief The async output object abstraction smart pointer.

        /// \brief The async I/O object abstraction.
        class io_object_mock
            : public virtual io_object,
              public virtual object_base_mock
        {
        public:
            io_object_mock(io::bus_ptr bus, io::file_descriptor_t fd);
            /// @brief Make sure the object is correctly destructed
            ~io_object_mock() override;

        private:
            /// @brief Read available data asynchronously.
            /// Pure virtual function. Implement it in the inherited concrete I/O object class.
            /// @param buf The buffer to write the recieved data to
            /// @param buf_len The length of the \p buf
            /// @return The \ref result_type with length of data read or error occured
            io::input_object::result_type _async_read_some(value_t *buf, std::size_t buf_len) override;
            /// @brief Write data asynchronously.
            /// Pure virtual function. Implement it in the inherited concrete I/O object class.
            /// @param buf The buffer of data to write from
            /// @param buf_len The length of the \p buf
            /// @return The \ref result_type with length of data written or error occured
            io::output_object::result_type _async_write_some(const value_t *buf, std::size_t buf_len) override;
        };
    }
}

#endif // H_IO_TEST_OBJECT_MOCK_T
