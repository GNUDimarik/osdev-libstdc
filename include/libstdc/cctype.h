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

#ifndef OSDEV_STDLIBC_CTYPE_H
#define OSDEV_STDLIBC_CTYPE_H

#include "config.h"

__MAYBE_BEGIN_STD_NAMESPACE

#ifndef __STD_LIBC_TEST
__BEGIN_DECLS
#endif

// To make clang happy
#ifndef __exctype
#define __exctype(name) extern int name (int) __NOEXCEPT
#endif

/**
 * @defgroup ctype ctype routines
 * @ingroup  stdlib
 * @{
 */

/**
 * @brief   toascii - translate an integer to a 7-bit ASCII character
 * @param c         - integer for converting
 * @return          - c & 0x7f
 */
__exctype(toascii);

/**
 * @brief isacii - function tests whether c is a 7-bit US-ASCII character code.
 * @param c - character to test
 * @return non-zero if c is a 7-bit US-ASCII character code between 0 and octal 0177 inclusive; otherwise, it returns 0
 */
__exctype(isascii);

/**
 * @brief   isalnum - checks if a character is alphanumeric
 * @param c - character to test
 * @return true if a character is alphanumeric and false otherwise
 */
__exctype(isalnum);

/**
 * @brief isalpha - checks if a character is alphabetic
 * @param c - character to test
 * @return true if a character is alphabetic and false otherwise
 */
__exctype(isalpha);
/**
 * @brief  isblank - checks if a character is a blank character
 * @param  c - character to test
 * @return true if a character is a blank character and false otherwise
 */
__exctype(isblank);

/**
 * @brief iscntrl - checks if a character is a control character
 * @param c - character to test
 * @return true if a character is a control character and false otherwise
 */
__exctype(iscntrl);

/**
 * @brief isdigit - checks if a character is a digit
 * @param c - character to test
 * @return true if a character is a digit and false otherwise
 */
__exctype(isdigit);

/**
 * @brief isgraph - checks if a character is a graphical character
 * @param c - character to test
 * @return true if a character is a graphical character and false otherwise
 */
__exctype(isgraph);

/**
 * @brief islower - checks if a character is lowercase
 * @param c - character to test
 * @return true if a character is lowercase and false otherwise
 */
__exctype(islower);
/**
 * @brief isprint - checks if a character is a printing character
 * @param c - character to test
 * @return true if a character is a printing character and false otherwise
 */
__exctype(isprint);

/**
 * @brief  ispunct - checks if a character is a punctuation character
 * @param  c - character to test
 * @return true if a character is a punctuation character and false otherwise
 */
__exctype(ispunct);

/**
 * @brief isspace - checks if a character is a space character
 * @param c - character to test
 * @return true if a character is a space character and false otherwise
 */
__exctype(isspace);

/**
 * @brief  isupper - checks if a character is an uppercase character
 * @param  c - character to test
 * @return true if a character is an uppercase character and false otherwise
 */
__exctype(isupper);

/**
 * @brief  isxdigit - checks if a character is a hexadecimal character
 * @param  c - character to test
 * @return true if a character is a hexadecimal character and false otherwise
 */
__exctype(isxdigit);

/**
 * @brief  tolower - converts a character to lowercase
 * @param  c - character to test
 * @return converted a character to lowercase
 */
__exctype(tolower);

/**
 * @brief  toupper - converts a character to uppercase
 * @param  c - character to test
 * @return converted a character to uppercase
 */
__exctype(toupper);

#ifndef __STD_LIBC_TEST
__END_DECLS
#endif

__MAYBE_END_STD_NAMESPACE
/** @} */
#endif