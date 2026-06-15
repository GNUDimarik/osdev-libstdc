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

#include "posix_strings.h"
#include <string.h>
#include <ctype.h>

__BEGIN_STDLIB_TEST_NAMESPACE

size_t strlcpy(char *__restrict__ dst, const char *__restrict__ src, size_t size)
{
    size_t len = strlen(src);

    if (len + 1 < size) {
        memcpy(dst, src, len + 1);
    }
    else if (size > 0) {
        memcpy(dst, src, size - 1);
        dst[size - 1] = '\0';
    }

    return len;
}

size_t strlcat(char *__restrict__ dst, const char *__restrict__ src, size_t dstsize)
{
    char *d = dst;
    const char *s = src;
    size_t n = dstsize;
    size_t dst_len = 0;

    for (; n > 0 && *d; n--, d++) {}
    dst_len = d - dst;
    n = dstsize - dst_len;

    if (n == 0) {
        return strlen(src) + dst_len;
    }

    while (*s) {
        if (n > 1) {
            *d++ = *s;
            n--;
        }
        s++;
    }

    *d = '\0';
    return dst_len + (s - src);
}

int strcasecmp(const char *first, const char *second)
{
    auto f = const_cast < char * > (first);
    char *s = const_cast<char *> (second);

    do {
        if (tolower(*f) != tolower(*s)) {
            return tolower(*f) - tolower(*s);
        }
    }
    while (*f++ && *s++);

    return 0;
}

int strncasecmp(const char *first, const char *second, size_t n)
{
    size_t i = 0;

    for (i = 0; i < n; i++) {
        if (tolower(first[i]) != tolower(second[i])) {
            return tolower(first[i]) - tolower(second[i]);
        }

        if (first[i] == '\0' || second[i] == '\0') {
            break;
        }
    }

    return 0;
}

int ffs(int val)
{
    int i = 0;

    if (val != 0) {
        for (i = 1; !(val & 1); i++) {
            val = static_cast<unsigned int>( val) >> 1;
        }
    }

    return i;
}

int ffsl(long int val)
{
    int i = 0;

    if (val != 0) {
        for (i = 1; !(val & 1); i++) {
            val = static_cast<unsigned long int> (val) >> 1;
        }
    }

    return i;
}

int ffsll(long long int val)
{
    int i = 0;

    if (val != 0) {
        for (i = 1; !(val & 1); i++) {
            val = static_cast<unsigned long long int> (val) >> 1;
        }
    }

    return i;
}
__END_STDLIB_TEST_NAMESPACE