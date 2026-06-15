#ifndef STDLIBC_CONFIG_H
#define STDLIBC_CONFIG_H

/* To make clang happy */
#ifndef __NOEXCEPT
#   if defined(__cplusplus)
#       define __NOEXCEPT noexcept
#   else
#       define __DUX_NOEXCEPT
#   endif
#endif

#if defined (__cplusplus)
#   define __EXTERN extern "C"
#else
#   define __EXTERN extern
#endif

#ifndef __BEGIN_DECLS
#   if defined (__cplusplus)
#   define __BEGIN_DECLS extern "C" {
#       else
#   define __BEGIN_DECLS
#       endif /* __cplusplus */
#endif /* #ifndef __BEGIN_DECLS */

#ifndef __END_DECLS
#   if defined (__cplusplus)
#       define __END_DECLS }
#   else
#       define __END_DECLS
#endif /* __cplusplus */

#endif /* #ifndef __END_DECLS */

#ifndef __STD_LIBC_TEST
#   define  __STD_NAMESPACE std
#   define  __BEGIN_STDLIB_TEST_NAMESPACE
#   define __END_STDLIB_TEST_NAMESPACE
#   define __STDLIBC_TEST_NAMESPACE
#else
#   define __STD_NAMESPACE __my_stdlibc_std
#   define __STDLIBC_TEST_NAMESPACE __my_stdlibc_std_test
#   define __BEGIN_STDLIB_TEST_NAMESPACE namespace __STDLIBC_TEST_NAMESPACE {
#   define __END_STDLIB_TEST_NAMESPACE }
#endif

#define __BEGIN_STD_NAMESPACE namespace __STD_NAMESPACE {
#define __END_STD_NAMESPACE }

#ifdef __STD_LIBC_TEST
#   define __MAYBE_BEGIN_STD_NAMESPACE __BEGIN_STD_NAMESPACE
#else
#   define __MAYBE_BEGIN_STD_NAMESPACE
#endif

#ifdef __STD_LIBC_TEST
#   define __MAYBE_END_STD_NAMESPACE __END_STD_NAMESPACE
#else
#   define __MAYBE_END_STD_NAMESPACE
#endif

#endif //STDLIBC_CONFIG_H
