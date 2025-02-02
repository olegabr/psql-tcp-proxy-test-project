/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_OBJECT_T
#define H_IO_OBJECT_T

#include "fd.hpp"
#include "bus.hpp"
#include "error.hpp" // io::error

#include <cstddef> // std::size_t
#include <functional>
#include <vector>
#include <memory>
#include <variant>

/// \brief The input/output library namespace
namespace io
{
	/// \brief The async I/O object abstraction common functionality.
	/// This class is abstract and internal. Should not be used by client code.
	/// \internal
	class object_base
	{
	public:
		/// @brief The data type to be read/written.
		/// It is used as value_t* so, even void is good.
		using value_t = void;

		/// @brief Get the underlying file descriptor value for this I/O object
		/// @return The underlying file descriptor value for this I/O object
		file_descriptor_t get_fd() const;
		/// @brief Get the underlying \ref io::bus object for this I/O object
		/// @return The underlying \ref io::bus object for this I/O object
		const bus_ptr &get_bus();

		/// @brief The convenience method to simplify \ref io::bus::add_fd method call
		/// @param cb The callback function to handle I/O bus events
		void add_bus_callback(const io::bus::callback_t &cb);
		/// @brief The convenience method to simplify \ref io::bus::del_fd method call
		void del_bus_fd();
		/// @brief The convenience method to simplify \ref io::bus::del_fd_callbacks method call
		void del_bus_fd_callbacks();

	protected:
		/// @brief Make sure the object is correctly destructed
		virtual ~object_base();

	private:
		/// @brief Get the underlying file descriptor value for this I/O object
		/// Pure virtual function. Implement it in the inherited concrete I/O object class.
		/// @return The underlying file descriptor value for this I/O object
		virtual io::file_descriptor_t _get_fd() const = 0;
		/// @brief Get the underlying \ref io::bus object for this I/O object
		/// Pure virtual function. Implement it in the inherited concrete I/O object class.
		/// @return The underlying \ref io::bus object for this I/O object
		virtual const io::bus_ptr &_get_bus() = 0;
	};

	/// \brief The async input object abstraction.
	class input_object
		: public virtual object_base
	{
	public:
		/// @brief The I/O operation successful result param variant for callback function type.
		struct success_result_type
		{
			/// @brief The underlying file descriptor value for this I/O result
			io::file_descriptor_t fd;
			/// @brief The buffer to write the recieved data to
			value_t *buf;
			/// @brief The length of the \p buf
			std::size_t buf_len;
		};
		/// @brief The I/O operation result param for callback function type.
		using result_type = std::variant<success_result_type, io::error>;
		/// @brief The I/O operation result callback function type.
		/// The file descriptor,
		/// data buffer pointer and bytes recieved or sent count
		/// are provided for the callback when called
		using callback_t = std::function<void(const result_type &)>;

		/// @brief Read available data asynchronously.
		/// @param buf The buffer to write the recieved data to
		/// @param buf_len The length of the \p buf
		/// @return The \ref result_type with length of data read or error occured
		result_type async_read_some(value_t *buf, std::size_t buf_len);

	protected:
		/// @brief Make sure the object is correctly destructed
		virtual ~input_object();

	private:
		/// @brief Read available data asynchronously.
		/// Pure virtual function. Implement it in the inherited concrete I/O object class.
		/// @param buf The buffer to write the recieved data to
		/// @param buf_len The length of the \p buf
		/// @return The \ref result_type with length of data read or error occured
		virtual result_type _async_read_some(value_t *buf, std::size_t buf_len) = 0;
	};
	/// \brief The async input object abstraction smart pointer.
	using input_object_ptr = std::shared_ptr<input_object>;

	/// \brief The async output object abstraction.
	class output_object
		: public virtual object_base
	{
	public:
		/// @brief The I/O operation successful result param variant for callback function type.
		struct success_result_type
		{
			/// @brief The underlying file descriptor value for this I/O result
			io::file_descriptor_t fd;
			/// @brief The buffer to read the data from
			const value_t *buf;
			/// @brief The length of the \p buf
			std::size_t buf_len;
		};
		/// @brief The I/O operation result param for callback function type.
		using result_type = std::variant<success_result_type, io::error>;
		/// @brief The I/O operation result callback function type.
		/// The file descriptor,
		/// data buffer pointer and bytes recieved or sent count
		/// are provided for the callback when called
		using callback_t = std::function<void(const result_type &)>;

		/// @brief Write data asynchronously.
		/// @param buf The buffer of data to write from
		/// @param buf_len The length of the \p buf
		/// @return The \ref result_type with length of data written or error occured
		result_type async_write_some(const value_t *buf, std::size_t buf_len);

	protected:
		/// @brief Make sure the object is correctly destructed
		virtual ~output_object();

	private:
		/// @brief Write data asynchronously.
		/// Pure virtual function. Implement it in the inherited concrete I/O object class.
		/// @param buf The buffer of data to write from
		/// @param buf_len The length of the \p buf
		/// @return The \ref result_type with length of data written or error occured
		virtual result_type _async_write_some(const value_t *buf, std::size_t buf_len) = 0;
	};
	/// \brief The async output object abstraction smart pointer.
	using output_object_ptr = std::shared_ptr<output_object>;

	/// \brief The async I/O object abstraction.
	class io_object
		: public input_object,
		  public output_object
	{
	protected:
		/// @brief Make sure the object is correctly destructed
		virtual ~io_object();
	};
	/// \brief The async I/O object abstraction smart pointer.
	using io_object_ptr = std::shared_ptr<io_object>;

	/// \brief helper type for the visitor #4
	/// See https://en.cppreference.com/w/cpp/utility/variant/visit
	template <class... Ts>
	struct make_visitor : Ts...
	{
		using Ts::operator()...;
	};
	// explicit deduction guide (not needed as of C++20)
	template <class... Ts>
	make_visitor(Ts...) -> make_visitor<Ts...>;
}

#endif // H_IO_OBJECT_T
