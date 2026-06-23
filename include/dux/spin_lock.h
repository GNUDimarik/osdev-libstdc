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

class spin_lock final
{
public:
    static constexpr const int kFree = 0;
    static constexpr const int kLocked = 1;

    spin_lock() = default;
    spin_lock(const spin_lock &) = delete;
    spin_lock &operator=(const spin_lock &) = delete;

    bool try_lock()
    {
        return __atomic_exchange_n(&_M_value, kLocked, __ATOMIC_ACQUIRE) == kFree;
    }

    void lock()
    {
        while (__atomic_exchange_n(&_M_value, kLocked, __ATOMIC_ACQUIRE) != kFree) {
            while (__atomic_load_n(&_M_value, __ATOMIC_RELAXED) != kFree) {
                cpu_relax();
            }
        }
    }

    void unlock()
    {
        __atomic_store_n(&_M_value, kFree, __ATOMIC_RELEASE);
    }
private:
    alignas(__GCC_DESTRUCTIVE_SIZE) int _M_value{kFree};
};
}

#endif //OSDEV_LIB_STDC_DUX_SPIN_LOCK_H
