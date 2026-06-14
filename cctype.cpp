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

#include "cctype.h"

__MAYBE_BEGIN_STD_NAMESPACE

int toascii(int c) __NOEXCEPT
{
    return (c & 0x7f);
}

int isascii(int c) __NOEXCEPT
{
    return c >= 0 && c <= 127;
}

int isalnum(int c) __NOEXCEPT
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

int isalpha(int c) __NOEXCEPT
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isblank(int c) __NOEXCEPT
{
    return c == 9 || c == 32;
}

int iscntrl(int c) __NOEXCEPT
{
    return (c >= 0 && c <= 31) || c == 127;
}

int isdigit(int c) __NOEXCEPT
{
    return c >= '0' && c <= '9';
}

int isgraph(int c) __NOEXCEPT
{
    return c >= 33 && c <= 126;
}

int islower(int c) __NOEXCEPT
{
    return c >= 'a' && c <= 'z';
}

int isprint(int c) __NOEXCEPT
{
    return c >= 32 && c <= 126;
}

int ispunct(int c) __NOEXCEPT
{
    return (c >= 33 && c <= 47) || (c >= 58 && c <= 64)
        || (c >= 91 && c <= 96) || (c >= 123 && c <= 126);
}

int isspace(int c) __NOEXCEPT
{
    return (c >= 9 && c <= 13) || c == 32;
}

int isupper(int c) __NOEXCEPT
{
    return c >= 'A' && c <= 'Z';
}

int isxdigit(int c) __NOEXCEPT
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

int tolower(int c) __NOEXCEPT
{
    if (c >= 'A' && c <= 'Z') {
        return c + ('a' - 'A');
    }

    return c;
}

int toupper(int c) __NOEXCEPT
{
    if (c >= 'a' && c <= 'z') {
        return c - ('a' - 'A');
    }

    return c;
}

__MAYBE_END_STD_NAMESPACE