/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_IP_ADDRINFO_T
#define H_IO_IP_ADDRINFO_T

#include <iterator>

#include <netdb.h>

/// \brief The input/output library namespace
namespace io
{
    /// \brief The ip subnamespace of the input/output library
    namespace ip
    {
        class v4;
        /// \brief The system specific namespace
        namespace system
        {
            class addrinfo_t;
            void swap(addrinfo_t &&, addrinfo_t &&);

            /// \brief Wrapper around the addrinfo struct
            /// Used to optimize new connection socket creation
            class addrinfo_t
            {
            public:
                explicit addrinfo_t(const v4 &address);
                ~addrinfo_t() noexcept;

                /// \brief No copy is allowed for resource wrappers
                addrinfo_t(const addrinfo_t &) = delete;
                /// \brief No copy is allowed for resource wrappers
                addrinfo_t &operator=(const addrinfo_t &) = delete;

                /// \brief Resource wrappers can be moved
                addrinfo_t(addrinfo_t &&) noexcept;
                /// \brief Resource wrappers can be moved
                addrinfo_t &operator=(addrinfo_t &&) noexcept;
                /// \brief Resource wrappers can be swapped
                friend void swap(addrinfo_t &&, addrinfo_t &&);

                /// \brief The Constant iterator wrapper to enumerate addrinfo structures
                class const_iterator
                    : public std::iterator<
                          std::input_iterator_tag, // iterator_category
                          struct addrinfo,         // value_type
                          std::ptrdiff_t,          // difference_type
                          const struct addrinfo *, // pointer
                          const struct addrinfo &  // reference
                          >
                {
                public:
                    explicit const_iterator(struct addrinfo *p) : _p(p) {}
                    const_iterator &operator++()
                    {
                        _p = _p->ai_next;
                        return *this;
                    }
                    const_iterator operator++(int)
                    {
                        const_iterator retval = *this;
                        ++(*this);
                        return retval;
                    }
                    bool operator==(const_iterator other) const { return _p == other._p; }
                    bool operator!=(const_iterator other) const { return !(*this == other); }
                    reference operator*() const { return *_p; }
                    pointer operator->() const { return _p; }

                private:
                    struct addrinfo *_p;
                };
                /// \brief The collection interface to enumerate addrinfo structures
                /// \return The \ref const_iterator for the first addrinfo struct available
                const_iterator begin() const { return const_iterator(_addrinfo); }
                /// \brief The collection interface to enumerate addrinfo structures
                /// \return The \ref const_iterator that marks the end of the collection
                const_iterator end() const { return const_iterator(nullptr); }

            private:
                /// \brief The wrapped resurce
                struct addrinfo *_addrinfo;
            };
        }
    }
}

#endif // H_IO_IP_ADDRINFO_T
