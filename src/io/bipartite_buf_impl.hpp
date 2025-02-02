/**************************************************************
 * @see https://github.com/DNedic/lockfree/blob/main/lockfree/spsc/bipartite_buf_impl.hpp
 * @file bipartite_buf_impl.hpp
 * @brief A bipartite buffer implementation written in standard
 * c++11 suitable for both low-end microcontrollers all the way
 * to HPC machines. Lock-free for single consumer single
 * producer scenarios.
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
#ifndef H_IO_UTIL_BIPARTITE_BUFFER_IMPL_T
#define H_IO_UTIL_BIPARTITE_BUFFER_IMPL_T

#include "bipartite_buf.hpp"

#include <algorithm>
#include <cassert>

namespace io
{
    namespace util
    {
        /********************** PUBLIC METHODS ************************/

        template <typename T, size_t size, typename ThreadSafetyStrategy>
        bipartite_buffer<T, size, ThreadSafetyStrategy>::bipartite_buffer()
            : _write_wrapped(false), _read_wrapped(false), _r(0U), _w(0U), _i(0U) {}

        template <typename T, size_t size, typename ThreadSafetyStrategy>
        T *bipartite_buffer<T, size, ThreadSafetyStrategy>::write_acquire(const size_t free_required)
        {
            /* Preload variables with adequate memory ordering */
            const size_t w = ThreadSafetyStrategy::load(_w, std::memory_order_relaxed);
            const size_t r = ThreadSafetyStrategy::load(_r, std::memory_order_acquire);

            const size_t free = _calc_free(w, r);
            const size_t linear_space = size - w;
            const size_t linear_free = std::min(free, linear_space);

            /* Try to find enough linear space until the end of the buffer */
            if (free_required <= linear_free)
            {
                return &_data[w];
            }

            /* If that doesn't work try from the beginning of the buffer */
            if (free_required <= free - linear_free)
            {
                _write_wrapped = true;
                return &_data[0];
            }

            /* Could not find free linear space with required size */
            return nullptr;
        }

        template <typename T, size_t size, typename ThreadSafetyStrategy>
        void bipartite_buffer<T, size, ThreadSafetyStrategy>::write_release(const size_t written)
        {
            size_t w = ThreadSafetyStrategy::load(_w, std::memory_order_relaxed);

            /* If the write wrapped set the invalidate index and reset write index*/
            size_t i;
            if (_write_wrapped)
            {
                _write_wrapped = false;
                i = w;
                w = 0U;
            }
            else
            {
                i = ThreadSafetyStrategy::load(_i, std::memory_order_relaxed);
            }

            /* Increment the write index */
            assert(w + written <= size);
            w += written;

            /* If we wrote over invalidated parts of the buffer move the invalidate
             * index
             */
            if (w > i)
            {
                i = w;
            }

            /* Wrap to 0 if needed */
            if (w == size)
            {
                w = 0U;
            }

            /* Store the indexes with adequate memory ordering */
            ThreadSafetyStrategy::store(_i, i, std::memory_order_relaxed);
            ThreadSafetyStrategy::store(_w, w, std::memory_order_release);
        }

        template <typename T, size_t size, typename ThreadSafetyStrategy>
        std::pair<T *, size_t> bipartite_buffer<T, size, ThreadSafetyStrategy>::read_acquire()
        {
            /* Preload variables with adequate memory ordering */
            const size_t r = ThreadSafetyStrategy::load(_r, std::memory_order_relaxed);
            const size_t w = ThreadSafetyStrategy::load(_w, std::memory_order_acquire);

            /* When read and write indexes are equal, the buffer is empty */
            if (r == w)
            {
                return std::make_pair(nullptr, 0U);
            }

            /* Simplest case, read index is behind the write index */
            if (r < w)
            {
                return std::make_pair(&_data[r], w - r);
            }

            /* Read index reached the invalidate index, make the read wrap */
            const size_t i = ThreadSafetyStrategy::load(_i, std::memory_order_relaxed);
            if (r == i)
            {
                _read_wrapped = true;
                return std::make_pair(&_data[0], w);
            }

            /* There is some data until the invalidate index */
            return std::make_pair(&_data[r], i - r);
        }

        template <typename T, size_t size, typename ThreadSafetyStrategy>
        void bipartite_buffer<T, size, ThreadSafetyStrategy>::read_release(const size_t read)
        {
            /* If the read wrapped, overflow the read index */
            size_t r;
            if (_read_wrapped)
            {
                _read_wrapped = false;
                r = 0U;
            }
            else
            {
                r = ThreadSafetyStrategy::load(_r, std::memory_order_relaxed);
            }

            /* Increment the read index and wrap to 0 if needed */
            assert(r + read <= size);
            r += read;
            if (r == size)
            {
                r = 0U;
            }

            /* Store the indexes with adequate memory ordering */
            ThreadSafetyStrategy::store(_r, r, std::memory_order_release);
        }

        /********************* PRIVATE METHODS ************************/

        template <typename T, size_t size, typename ThreadSafetyStrategy>
        size_t bipartite_buffer<T, size, ThreadSafetyStrategy>::_calc_free(const size_t w, const size_t r)
        {
            if (r > w)
            {
                return (r - w) - 1U;
            }
            else
            {
                return (size - (w - r)) - 1U;
            }
        }

    } /* namespace util */
} /* namespace io */

#endif // H_IO_UTIL_BIPARTITE_BUFFER_IMPL_T
