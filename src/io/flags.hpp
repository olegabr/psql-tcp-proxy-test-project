/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_FLAGS_T
#define H_IO_FLAGS_T

#include <iosfwd>
#include <cstdint>

/// \brief The input/output library namespace
namespace io
{
    /// \brief Implementation details
    namespace detail
    {
        /// \brief The I/O flags used in I/O callbacks to indicate the event type.
        enum class _flags_enum : uint8_t
        {
            /// @brief The empty flags value
            empty = 0u,
            /// @brief Indicates the input type event
            in = 1u,
            /// @brief Indicates the output type event
            out = 2u,
            /// @brief Indicates both the input and the output type event
            inout = 3u,
            /// @brief Indicates that some I/O error occured
            error = 4u
        };

    }

    class flags;

    /// @brief The std::ostream output operator overload
    /// to output the \p fl value in a human readable format.
    /// @param os The output stream object
    /// @param fl The \ref io::flags value to output
    /// @return The output stream object \p os
    std::ostream &operator<<(std::ostream &os, flags fl);

    /// @brief The bitwise OR with assign operator
    /// @param lh The value to modify
    /// @param rh The value to add to \p lh
    /// @return Reference to the \p lh
    constexpr flags &operator|=(flags &lh, flags rh);

    /// @brief The bitwise OR operator
    /// @param lh The left value
    /// @param rh The right value
    /// @return The biwise OR of \p lh and \p rh
    constexpr flags operator|(flags lh, flags rh);

    /// @brief The bitwise AND with assign operator
    /// @param lh The value to modify
    /// @param rh The value to intersect with \p lh
    /// @return Reference to the \p lh
    constexpr flags &operator&=(flags &lh, flags rh);

    /// @brief The bitwise AND operator
    /// @param lh The left value
    /// @param rh The right value
    /// @return The biwise AND of \p lh and \p rh
    constexpr flags operator&(flags lh, flags rh);

    /// \brief The I/O flags used in I/O callbacks to indicate the event type.
    class flags
    {
    public:
        /// @brief The empty flags value
        static const flags empty;
        /// @brief Indicates the input type event
        static const flags in;
        /// @brief Indicates the output type event
        static const flags out;
        /// @brief Indicates both the input and the output type event
        static const flags inout;
        /// @brief Indicates that some I/O error occured
        static const flags error;

        constexpr explicit flags(detail::_flags_enum value = detail::_flags_enum::empty)
            : _value(static_cast<uint8_t>(value))
        {
        }

        constexpr bool test(flags other) const
        {
            if (0 == other._value)
            {
                return 0 == _value;
            }
            return other._value == (_value & other._value);
        }

        friend std::ostream &operator<<(std::ostream &os, flags fl);
        friend constexpr flags &operator|=(flags &lh, flags rh);
        friend constexpr flags operator|(flags lh, flags rh);
        friend constexpr flags &operator&=(flags &lh, flags rh);
        friend constexpr flags operator&(flags lh, flags rh);

    private:
        uint8_t _value;
    };

    inline constexpr flags flags::empty{detail::_flags_enum::empty};
    inline constexpr flags flags::in{detail::_flags_enum::in};
    inline constexpr flags flags::out{detail::_flags_enum::out};
    inline constexpr flags flags::inout{detail::_flags_enum::inout};
    inline constexpr flags flags::error{detail::_flags_enum::error};

    constexpr flags &operator|=(flags &lh, flags rh)
    {
        lh._value |= rh._value;
        return lh;
    }

    constexpr flags operator|(flags lh, flags rh)
    {
        lh |= rh;
        return lh;
    }

    constexpr flags &operator&=(flags &lh, flags rh)
    {
        lh._value &= rh._value;
        return lh;
    }

    constexpr flags operator&(flags lh, flags rh)
    {
        lh &= rh;
        return lh;
    }
}

#endif // H_IO_FLAGS_T
