/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_V4_T
#define H_IO_IP_V4_T

#include <cstdint>
#include <string>
#include <iosfwd>

/// \brief The input/output library namespace
namespace io
{
	/// \brief The IP protocol related abstractions namespace
	namespace ip
	{
		class v4;
		void swap(v4 &lh, v4 &rh) noexcept;
		std::ostream &operator<<(std::ostream &os, const v4 &address);

		/// @brief The IPv4 address wrapper
		class v4
		{
		public:
			/// @brief Construct the IPv4 address from host and port
			/// @param host The host string like "127.0.0.1"
			/// @param port The port string like "80"
			v4(std::string const &host, std::string const &port);

			/// @brief Copy constructor
			/// @param other The \ref io::ip::v4 object to copy construct from
			v4(const v4 &other) = default;
			/// @brief Copy construct by assignment
			/// @param other The \ref io::ip::v4 object to copy construct from
			/// @return This object
			// v4 &operator=(const v4 &other) = default;

			/// @brief Move constructor
			/// @param other The \ref io::ip::v4 object to move construct from
			v4(v4 &&other) noexcept;
			/// @brief Copy/Move construct by assignment
			/// @param other The \ref io::ip::v4 object to move construct from
			/// @return This object
			v4 &operator=(v4 other) noexcept;

			/// @brief Get the host string for this address
			/// @return The host string for this address
			const std::string &host() const
			{
				return _host;
			}

			/// @brief Get the port number for this address
			/// @return The port number for this address
			uint16_t port() const
			{
				return _port;
			}

		private:
			/// @brief Swap two \ref io::ip::v4 objects
			/// @param lh Left object to swap
			/// @param rh Right object to swap
			friend void swap(v4 &lh, v4 &rh) noexcept;

			/// @brief The std::ostream output operator overload
			/// to output the \p address in a human readable format.
			/// @param os The output stream object
			/// @param address The \ref io::ip::v4 value to output
			/// @return The output stream object \p os
			friend std::ostream &operator<<(std::ostream &os, const v4 &address);

			/// @brief The host string for this address
			std::string _host;
			/// @brief The port number for this address
			uint16_t _port;
		};
	}
}

#endif // H_IO_IP_V4_T
