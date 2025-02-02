/**************************************************************
 * @see https://github.com/DNedic/lockfree/blob/main/lockfree/spsc/bipartite_buf.hpp
 * @file bipartite_buf.hpp
 * @brief A bipartite buffer implementation written in
 * standard c++11 suitable for all systems, from low-end
 * microcontrollers to HPC machines.
 * Lock-free for single consumer single producer scenarios.
 **************************************************************/

/**************************************************************
 * Copyright (c) 2023 Djordje Nedic
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to
 * whom the Software is furnished to do so, subject to the
 * following conditions:
 *
 * The above copyright notice and this permission notice shall
 * be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of lockfree
 *
 * Author:          Djordje Nedic <nedic.djordje2@gmail.com>
 * Version:         v2.0.9
 **************************************************************/

/************************** INCLUDE ***************************/
#ifndef H_IO_UTIL_BIPARTITE_BUFFER_T
#define H_IO_UTIL_BIPARTITE_BUFFER_T

#include <atomic>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace io
{
    namespace util
    {
        /*************************** TYPES ****************************/

        /// @brief A no-op thread safety strategy for \ref io::util::bipartite_buffer
        /// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
        struct thread_safety_noop
        {
            using size_type = size_t;
            using safe_size_type = size_t;
            static void store(
                safe_size_type &value,
                size_type desired,
                std::memory_order /*order*/) noexcept
            {
                value = desired;
            }
            static size_type load(
                safe_size_type &value,
                std::memory_order /*order*/) noexcept
            {
                return value;
            }
        };

        /// @brief Atomic thread safety strategy for \ref io::util::bipartite_buffer
        /// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
        struct thread_safety_atomic
        {
            using size_type = size_t;
            using safe_size_type = std::atomic_size_t;
            static void store(
                safe_size_type &value,
                size_type desired,
                std::memory_order order) noexcept
            {
                value.store(desired, order);
            }
            static size_type load(
                safe_size_type &value,
                std::memory_order order) noexcept
            {
                return value.load(order);
            }
        };

        /// @brief A bipartite buffer implementation for efficient read/write
        template <typename T, size_t size, typename ThreadSafetyStrategy = thread_safety_noop>
        class bipartite_buffer
        {
            static_assert(std::is_trivial<T>::value, "The type T must be trivial");
            static_assert(size > 2, "Buffer size must be bigger than 2");

            /********************** PUBLIC METHODS ************************/
        public:
            bipartite_buffer();

            /**
             * @brief Acquires a linear region in the bipartite buffer for writing
             * Should only be called from the producer thread.
             * @param[in] free_required Free linear space in the buffer required
             * @retval Pointer to the beginning of the linear space, nullptr if no space
             */
            T *write_acquire(size_t free_required);

            /**
             * @brief Releases the bipartite buffer after a write
             * Should only be called from the producer thread.
             * @param[in] written Elements written to the linear space
             * @retval None
             */
            void write_release(size_t written);

            /**
             * @brief Acquires a linear region in the bipartite buffer for reading.
             * Should only be called from the consumer thread.
             * @retval Pair containing the pointer to the beginning of the area and
             * elements available
             */
            std::pair<T *, size_t> read_acquire();

            /**
             * @brief Releases the bipartite buffer after a read
             * Should only be called from the consumer thread.
             * @param[in] read Elements read from the linear region
             * @retval None
             */
            void read_release(size_t read);

            /********************* PRIVATE METHODS ************************/
        private:
            static size_t _calc_free(const size_t w, const size_t r);

            /********************** PRIVATE MEMBERS ***********************/
        private:
            T _data[size];                                    /**< Data array */
            bool _write_wrapped;                              /**< Write wrapped flag, used only in the producer */
            bool _read_wrapped;                               /**< Read wrapped flag, used only in the consumer */
            typename ThreadSafetyStrategy::safe_size_type _r; /**< Read index */
            typename ThreadSafetyStrategy::safe_size_type _w; /**< Write index */
            typename ThreadSafetyStrategy::safe_size_type _i; /**< Invalidated space index */
        };

    } /* namespace util */
} /* namespace io */

/************************** INCLUDE ***************************/

/* Include the implementation */
#include "bipartite_buf_impl.hpp"

#endif /* H_IO_UTIL_BIPARTITE_BUFFER_T */