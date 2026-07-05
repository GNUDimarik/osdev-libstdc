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
#include "memory.h"
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
        case memory_order_release:return memory_order_relaxed;

        case memory_order_acq_rel:return memory_order_acquire;

        default:return order;
    }
}

} // namespace detail

template<typename __Type, template<typename> typename __AtomicType>
__Type *__get_atomic_ptr(__AtomicType<__Type> *__a)
{
    return __a->_M_get_ptr();
}

template<typename __Type, template<typename> typename __AtomicType>
__Type *__get_atomic_ptr(volatile __AtomicType<__Type> *__a)
{
    return __a->_M_get_ptr();
}

template<typename __Type, template<typename> typename __AtomicType>
__Type *__get_atomic_ptr(const __AtomicType<__Type> *__a)
{
    return __a->_M_get_ptr();
}

template<typename __Type, template<typename> typename __AtomicType>
__Type *__get_atomic_ptr(const volatile __AtomicType<__Type> *__a)
{
    return __a->_M_get_ptr();
}

template<typename _Type, typename _HoldType>
struct _AtomicValHolder
{
    _AtomicValHolder() = delete;
    _AtomicValHolder(_HoldType __v)
        : _M_value(__v)
    {}

    _Type *_M_get_ptr()
    { return __STD_NAMESPACE::addressof(_M_value); }

    _Type *_M_get_ptr() const
    { return const_cast<_Type *>(__STD_NAMESPACE::addressof(_M_value)); }

    _Type *_M_get_ptr() volatile
    { return const_cast<_Type *>(__STD_NAMESPACE::addressof(_M_value)); }

    _Type *_M_get_ptr() const volatile
    { return const_cast<_Type *>(__STD_NAMESPACE::addressof(_M_value)); }

    _HoldType _M_value;
};

template<typename _Type, typename _Derived>
class basic_atomic
{
public:
    basic_atomic() = default;
    basic_atomic(const basic_atomic &) = delete;
    basic_atomic &operator=(const basic_atomic &) = delete;
    basic_atomic &operator=(const basic_atomic &) volatile = delete;

    _Type operator=(_Type __desired) noexcept
    {
        __atomic_store(_M_ptr(), &__desired, memory_order::memory_order_seq_cst);
        return __desired;
    }

    _Type operator=(_Type __desired) volatile noexcept
    {
        __atomic_store(_M_ptr(), &__desired, memory_order::memory_order_seq_cst);
        return __desired;
    }

    bool is_lock_free() const noexcept
    {
        return __atomic_is_lock_free(sizeof(_Type), _M_ptr());
    }

    bool is_lock_free() const volatile noexcept
    {
        return __atomic_is_lock_free(sizeof(_Type), _M_ptr());
    }

    void store(_Type __desired, __STD_NAMESPACE::memory_order __order =
    __STD_NAMESPACE::memory_order_seq_cst) noexcept
    {
        __atomic_store(_M_ptr(), &__desired, __order);
    }

    void store(_Type __desired, __STD_NAMESPACE::memory_order __order =
    __STD_NAMESPACE::memory_order_seq_cst) volatile noexcept
    {
        __atomic_store(_M_ptr(), &__desired, __order);
    }

    _Type load(__STD_NAMESPACE::memory_order __order
    = __STD_NAMESPACE::memory_order_seq_cst) const noexcept
    {
        _Type ret;
        __atomic_load(_M_ptr(), &ret, __order);
        return ret;
    }

    _Type load(__STD_NAMESPACE::memory_order __order
    = __STD_NAMESPACE::memory_order_seq_cst) const volatile noexcept
    {
        _Type ret;
        __atomic_load(_M_ptr(), &ret, __order);
        return ret;
    }

    operator _Type() const noexcept
    {
        _Type ret;
        __atomic_load(_M_ptr(), &ret, memory_order::memory_order_seq_cst);
        return ret;
    }

    operator _Type() const volatile noexcept
    {
        _Type ret;
        __atomic_load(_M_ptr(), &ret, memory_order::memory_order_seq_cst);
        return ret;
    }

    _Type exchange(_Type __desired, __STD_NAMESPACE::memory_order __order =
    __STD_NAMESPACE::memory_order_seq_cst) noexcept
    {
        _Type ret;
        __atomic_exchange(_M_ptr(), &__desired, &ret, __order);
        return ret;
    }

    _Type exchange(_Type __desired, __STD_NAMESPACE::memory_order __order =
    __STD_NAMESPACE::memory_order_seq_cst) volatile noexcept
    {
        _Type ret;
        __atomic_exchange(_M_ptr(), &__desired, &ret, __order);
        return ret;
    }

    bool compare_exchange_weak(_Type &expected, _Type __desired,
                               __STD_NAMESPACE::memory_order __success,
                               __STD_NAMESPACE::memory_order __failure) noexcept
    {
        return __atomic_compare_exchange(_M_ptr(), &expected, &__desired, true, __success, __failure);
    }

    bool compare_exchange_weak(_Type &expected, _Type __desired,
                               __STD_NAMESPACE::memory_order __success,
                               __STD_NAMESPACE::memory_order __failure) volatile noexcept
    {
        return __atomic_compare_exchange(_M_ptr(), &expected, &__desired, true, __success, __failure);
    }

    bool compare_exchange_weak(_Type &expected, _Type __desired,
                               __STD_NAMESPACE::memory_order __order =
                               __STD_NAMESPACE::memory_order_seq_cst) noexcept
    {
        return __atomic_compare_exchange(_M_ptr(),
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
        return __atomic_compare_exchange(_M_ptr(),
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
        return __atomic_compare_exchange(_M_ptr(), &expected, &__desired, false, __success, __failure);
    }

    bool compare_exchange_strong(_Type &expected, _Type __desired,
                                 __STD_NAMESPACE::memory_order __success,
                                 __STD_NAMESPACE::memory_order __failure) volatile noexcept
    {
        return __atomic_compare_exchange(_M_ptr(), &expected, &__desired, false, __success, __failure);
    }

    bool compare_exchange_strong(_Type &expected, _Type __desired,
                                 __STD_NAMESPACE::memory_order __order =
                                 __STD_NAMESPACE::memory_order_seq_cst) noexcept
    {
        return __atomic_compare_exchange(_M_ptr(),
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
        return __atomic_compare_exchange(_M_ptr(),
                                         &expected,
                                         &__desired,
                                         false,
                                         __order,
                                         detail::__cas_failure_order(__order));
    }
private:
    _Type *_M_ptr()
    {
        return __get_atomic_ptr(static_cast<_Derived *>(this));
    }

    _Type *_M_ptr() const
    {
        return __get_atomic_ptr(static_cast<const _Derived *>(this));
    }

    _Type *_M_ptr() const volatile
    {
        return __get_atomic_ptr(static_cast<const volatile _Derived *>(this));
    }

    _Type *_M_ptr() volatile
    {
        return __get_atomic_ptr(static_cast<volatile _Derived *>(this));
    }
};

template<typename _Type>
class atomic: _AtomicValHolder<_Type, _Type>, public basic_atomic<_Type, atomic<_Type>>
{
public:
    friend _Type *__get_atomic_ptr<>(atomic<_Type> *);
    friend _Type *__get_atomic_ptr<>(volatile atomic<_Type> *);
    friend _Type *__get_atomic_ptr<>(const atomic<_Type> *);
    friend _Type *__get_atomic_ptr<>(const volatile atomic<_Type> *);

    using basic_atomic<_Type, atomic<_Type>>::operator=;

    explicit atomic()
        : _AtomicValHolder<_Type, _Type>(_Type{})
    {}

    atomic(_Type __v)
        : _AtomicValHolder<_Type, _Type>(__v)
    {}
private:
    using _AtomicValHolder<_Type, _Type>::_M_get_ptr;
};

template<typename _Type>
class atomic_ref: _AtomicValHolder<_Type, _Type &>, public basic_atomic<_Type, atomic_ref<_Type>>
{
public:
    friend _Type *__get_atomic_ptr<>(atomic_ref<_Type> *);
    friend _Type *__get_atomic_ptr<>(volatile atomic_ref<_Type> *);
    friend _Type *__get_atomic_ptr<>(const atomic_ref<_Type> *);
    friend _Type *__get_atomic_ptr<>(const volatile atomic_ref<_Type> *);

    using basic_atomic<_Type, atomic<_Type>>::operator=;

    atomic_ref() = delete;
    explicit atomic_ref(_Type &__v)
        : _AtomicValHolder<_Type, _Type &>(__v)
    {}
private:
    using _AtomicValHolder<_Type, _Type &>::_M_get_ptr;
};

__END_STD_NAMESPACE

#endif //__INCLUDE_ATOMIC_H__