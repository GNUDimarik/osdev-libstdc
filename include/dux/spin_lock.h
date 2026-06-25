/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dmitry Adzhiev <dmitry.adjiev@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef OSDEV_LIB_STDC_DUX_SPIN_LOCK_H
#define OSDEV_LIB_STDC_DUX_SPIN_LOCK_H

#include <asm/cpu.h>
#include <config.h>
#include "../atomic.h"

__BEGIN_STD_NAMESPACE

template<typename _Lock>
class lock_guard final
{
public:
    lock_guard(const lock_guard &) = delete;
    lock_guard &operator=
        (const lock_guard &) = delete;

    lock_guard(_Lock &__lock)
        : _M_lock(__lock)
    {
        _M_lock.lock();
    }

    ~lock_guard()
    {
        _M_lock.unlock();
    }
private:
    _Lock &_M_lock;
};

__END_STD_NAMESPACE

namespace dux
{

static constexpr int kFree = 0;

static constexpr int kLocked = 1;

class spin_lock final
{
public:
    spin_lock() = default;
    spin_lock(const spin_lock &) = delete;
    spin_lock &operator=(const spin_lock &) = delete;

    bool try_lock()
    {
        int expected = kFree;
        return _M_value.compare_exchange_strong(expected, kLocked, __STD_NAMESPACE::memory_order_acquire);
    }

    void lock()
    {
        for (;;) {
            while (_M_value.load(__STD_NAMESPACE::memory_order_relaxed) == kLocked) {
                cpu_relax();
            }

            int expected = kFree;

            if (_M_value.compare_exchange_strong(expected, kLocked, __STD_NAMESPACE::memory_order_acquire)) {
                return;
            }

            cpu_relax();
        }
    }

    void unlock()
    {
        _M_value.store(kFree, __STD_NAMESPACE::memory_order_release);
    }
private:
    alignas(__STD_NAMESPACE::hardware_destructive_interference_size) __STD_NAMESPACE::atomic<int> _M_value = kFree;
};
} // namespace dux

#endif //OSDEV_LIB_STDC_DUX_SPIN_LOCK_H
