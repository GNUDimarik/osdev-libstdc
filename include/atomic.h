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

#ifndef __INCLUDE_ATOMIC_H__
#define __INCLUDE_ATOMIC_H__

#include "config.h"
#include "abort.h"
#include <stddef.h>

__BEGIN_STD_NAMESPACE

inline constexpr size_t hardware_destructive_interference_size = __GCC_DESTRUCTIVE_SIZE;
inline constexpr size_t hardware_constructive_interference_size = __GCC_CONSTRUCTIVE_SIZE;

enum memory_order
{
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
};

namespace detail
{

static memory_order __cas_failure_order(memory_order order) noexcept
{
    switch (order) {
        case memory_order_relaxed:
            return memory_order_relaxed;

        case memory_order_acquire:
            return memory_order_acquire;

        case memory_order_release:
            return memory_order_relaxed;

        case memory_order_acq_rel:
            return memory_order_acquire;

        case memory_order_seq_cst:
            return memory_order_seq_cst;
    }

    __STD_NAMESPACE::abort();
}

} // namespace detail

template<typename _Type>
class atomic
{
public:
    atomic(const atomic &) = delete;
    atomic &operator=(const atomic &) = delete;
    atomic &operator=(const atomic &) volatile = delete;

    atomic() = default;
    atomic(_Type __v)
        : _M_value(__v)
    {}

    _Type operator=(_Type __desired) noexcept
    {
        __atomic_store(&_M_value, &__desired, memory_order::memory_order_seq_cst);
        return __desired;
    }

    _Type operator=(_Type __desired) volatile noexcept
    {
        __atomic_store(&_M_value, &__desired, memory_order::memory_order_seq_cst);
        return __desired;
    }

    bool is_lock_free() const noexcept
    {
        return __atomic_is_lock_free(sizeof(_Type), nullptr);
    }

    bool is_lock_free() const volatile noexcept
    {
        return __atomic_is_lock_free(sizeof(_Type), nullptr);
    }

    void store(_Type __desired, __STD_NAMESPACE::memory_order __order =
    __STD_NAMESPACE::memory_order_seq_cst) noexcept
    {
        __atomic_store(&_M_value, &__desired, __order);
    }

    void store(_Type __desired, __STD_NAMESPACE::memory_order __order =
    __STD_NAMESPACE::memory_order_seq_cst) volatile noexcept
    {
        __atomic_store(&_M_value, &__desired, __order);
    }

    _Type load(__STD_NAMESPACE::memory_order __order
    = __STD_NAMESPACE::memory_order_seq_cst) const noexcept
    {
        _Type ret;
        __atomic_load(&_M_value, &ret, __order);
        return ret;
    }

    _Type load(__STD_NAMESPACE::memory_order __order
    = __STD_NAMESPACE::memory_order_seq_cst) const volatile noexcept
    {
        _Type ret;
        __atomic_load(&_M_value, &ret, __order);
        return ret;
    }

    operator _Type() const noexcept
    {
        _Type ret;
        __atomic_load(&_M_value, &ret, memory_order::memory_order_seq_cst);
        return ret;
    }

    operator _Type() const volatile noexcept
    {
        _Type ret;
        __atomic_load(&_M_value, &ret, memory_order::memory_order_seq_cst);
        return ret;
    }

    _Type exchange(_Type __desired, __STD_NAMESPACE::memory_order __order =
    __STD_NAMESPACE::memory_order_seq_cst) noexcept
    {
        _Type ret;
        __atomic_exchange(&_M_value, &__desired, &ret, __order);
        return ret;
    }

    _Type exchange(_Type __desired, __STD_NAMESPACE::memory_order __order =
    __STD_NAMESPACE::memory_order_seq_cst) volatile noexcept
    {
        _Type ret;
        __atomic_exchange(&_M_value, &__desired, &ret, __order);
        return ret;
    }

    bool compare_exchange_weak(_Type &expected, _Type __desired,
                               __STD_NAMESPACE::memory_order __success,
                               __STD_NAMESPACE::memory_order __failure) noexcept
    {
        return __atomic_compare_exchange(&_M_value, &expected, &__desired, true, __success, __failure);
    }

    bool compare_exchange_weak(_Type &expected, _Type __desired,
                               __STD_NAMESPACE::memory_order __success,
                               __STD_NAMESPACE::memory_order __failure) volatile noexcept
    {
        return __atomic_compare_exchange(&_M_value, &expected, &__desired, true, __success, __failure);
    }

    bool compare_exchange_weak(_Type &expected, _Type __desired,
                               __STD_NAMESPACE::memory_order __order =
                               __STD_NAMESPACE::memory_order_seq_cst) noexcept
    {
        return __atomic_compare_exchange(&_M_value,
                                         &expected,
                                         &__desired,
                                         true,
                                         __order,
                                         detail::__cas_failure_order(__order));
    }

    bool compare_exchange_weak(_Type &expected, _Type __desired,
                               __STD_NAMESPACE::memory_order __order =
                               __STD_NAMESPACE::memory_order_seq_cst) volatile noexcept
    {
        return __atomic_compare_exchange(&_M_value,
                                         &expected,
                                         &__desired,
                                         true,
                                         __order,
                                         detail::__cas_failure_order(__order));
    }

    bool compare_exchange_strong(_Type &expected, _Type __desired,
                                 __STD_NAMESPACE::memory_order __success,
                                 __STD_NAMESPACE::memory_order __failure) noexcept
    {
        return __atomic_compare_exchange(&_M_value, &expected, &__desired, false, __success, __failure);
    }

    bool compare_exchange_strong(_Type &expected, _Type __desired,
                                 __STD_NAMESPACE::memory_order __success,
                                 __STD_NAMESPACE::memory_order __failure) volatile noexcept
    {
        return __atomic_compare_exchange(&_M_value, &expected, &__desired, false, __success, __failure);
    }

    bool compare_exchange_strong(_Type &expected, _Type __desired,
                                 __STD_NAMESPACE::memory_order __order =
                                 __STD_NAMESPACE::memory_order_seq_cst) noexcept
    {
        return __atomic_compare_exchange(&_M_value,
                                         &expected,
                                         &__desired,
                                         false,
                                         __order,
                                         detail::__cas_failure_order(__order));
    }

    bool compare_exchange_strong
        (_Type &expected, _Type __desired,
         __STD_NAMESPACE::memory_order __order = __STD_NAMESPACE::memory_order_seq_cst) volatile noexcept
    {
        return __atomic_compare_exchange(&_M_value,
                                         &expected,
                                         &__desired,
                                         false,
                                         __order,
                                         detail::__cas_failure_order(__order));
    }
private:
    _Type _M_value{};
};

__END_STD_NAMESPACE

#endif //__INCLUDE_ATOMIC_H__
