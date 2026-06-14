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

#include "cstring.h"
#include <ctype.h>
#include "config.h"
#include "utils.h"

__MAYBE_BEGIN_STD_NAMESPACE
__BEGIN_DECLS

void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t length)
{
    auto *d = static_cast<unsigned char *> (dest);
    auto *s = static_cast<const unsigned char *> (src);

    if (d == s) {
        return d;
    }

    while (length-- > 0) {
        *d++ = *s++;
    }

    return dest;
}

void *memmove(void *dest, const void *src, size_t length)
{
    auto *d = static_cast<unsigned char *> (dest);
    auto *s = static_cast<const unsigned char *> (src);

    if (s >= d) {
        /* copy forward */
        while (length-- > 0) {
            *d++ = *s++;
        }
    }
    else if (s <= d) {
        /* copy backward */
        d += length;
        s += length;

        while (length-- > 0) {
            *--d = *--s;
        }
    }

    return dest;
}

void *memchr(const void *memory, int needle, size_t count)
{
    auto p = const_cast<unsigned char *>(static_cast<const unsigned char *> (memory));

    while (count-- > 0) {
        if (*p++ == (unsigned char) needle) {
            return --p;
        }
    }

    return nullptr;
}

void *memrchr(const void *mem, int needle, size_t count)
{
    auto s = const_cast<unsigned char *> (static_cast<const unsigned char *>(mem) + count - 1);

    while (count-- > 0) {
        if (*s-- == (unsigned char) needle) {
            return ++s;
        }
    }

    return nullptr;
}

int memcmp(const void *first, const void *second, size_t length)
{
    auto m1 = static_cast<const unsigned char *> (first);
    auto m2 = static_cast<const unsigned char *> (second);
    size_t i = 0;

    for (i = 0; i < length; i++) {
        if (m1[i] != m2[i]) {
            return m1[i] - m2[i];
        }
    }

    return 0;
}

void *memset(void *dest, int value, size_t length)
{
    auto *p = static_cast<unsigned char *> (dest);

    while (length-- > 0) {
        *p++ = (unsigned char) value;
    }

    return dest;
}

char *strcat(char *__restrict__ dest, const char *__restrict__ src)
{
    char *d = dest;
    const char *s = src;

    while (*d++) {}

    --d;

    while (*s) {
        *d++ = *s++;
    }

    *d = '\0';
    return dest;
}

char *strncat(char *dest, const char *src, size_t length)
{
    char *d = dest;
    const char *s = src;

    while (*d++) {}

    --d;

    while (*s && length-- > 0) {
        *d++ = *s++;
    }

    *d = '\0';
    return dest;
}

char *strchr(const char *str, int needle)
{
    char *s = (char *) str;

    do {
        if (*s == static_cast<char> (needle)) {
            return s;
        }
    }
    while (*s++);

    return nullptr;
}

char *strrchr(const char *str, int needle)
{
    const char *s = str;
    ptrdiff_t pos = -1;

    do {
        if (*s == needle) {
            pos = s - str;
        }
    }
    while (*s++);

    return (pos == -1 ? nullptr : (char *) str + pos);
}

int strcmp(const char *first, const char *second)
{
    char *f = const_cast<char *> (first);
    char *s = const_cast<char *> (second);

    do {
        if (*f != *s) {
            return *f - *s;
        }
    }
    while (*f++ && *s++);

    return 0;
}

int strncmp(const char *first, const char *second, size_t length)
{
    size_t i = 0;

    for (i = 0; i < length; i++) {
        if (first[i] != second[i]) {
            return first[i] - second[i];
        }

        if (first[i] == '\0' || second[i] == '\0') {
            break;
        }
    }

    return 0;
}

char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    char *s = const_cast<char *> (src);

    do {
        *d = *s;
    }
    while (*s++ && *d++);

    return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    char *d = dest;
    char *s = const_cast<char *> (src);

    while (n > 0) {
        if (*s != '\0') {
            *d++ = *s++;
        }
        else {
            *d++ = '\0';
        }

        n--;
    }

    return dest;
}

size_t strspn(const char *str, const char *accept)
{
    size_t res = 0;

    while (*str) {
        if (strchr(accept, *str++) != nullptr) {
            res++;
        }
        else {
            break;
        }
    }

    return res;
}

size_t strlen(const char *str)
{
    size_t res = 0;

    while (*str++ != '\0') {
        res++;
    }

    return res;
}

size_t strnlen(const char *str, size_t max_len)
{
    return min(strlen(str), max_len);
}

size_t strcspn(const char *str, const char *reject)
{
    const char *s = str;
    size_t res = 0;

    while (*s) {
        if (strchr(reject, *s++) == nullptr) {
            res++;
        }
        else {
            break;
        }
    }

    return res;
}

char *strpbrk(const char *str, const char *accept)
{
    char *s = (char *) str;

    while (*s) {
        if (strchr(accept, *s++) != nullptr) {
            return --s;
        }
    }

    return nullptr;
}

char *strstr(const char *haystack, const char *needle)
{
    size_t needle_len = strlen(needle);
    size_t haystack_len = strlen(haystack);
    size_t i = 0;

    if (!needle_len && !haystack_len) {
        return const_cast<char *> (haystack);
    }

    for (i = haystack_len; i >= needle_len; i--, haystack++) {
        if (strncmp(haystack, needle, needle_len) == 0) {
            return const_cast<char *> (haystack);
        }
    }

    return nullptr;
}

char *strtok(char *str, const char *delim)
{
    static char *old_str = nullptr;
    return strtok_r(str, delim, &old_str);
}

char *strtok_r(char *str, const char *delim,
               char **saveptr)
{
    char *old_str = saveptr ? *saveptr : nullptr;
    char *res = nullptr;

    if (!str) {
        if (!old_str) {
            return nullptr;
        }
    }
    else {
        old_str = str;
    }

    old_str += strspn(old_str, delim);

    if (*old_str == '\0') {
        return nullptr;
    }

    res = old_str++;
    old_str += strcspn(old_str, delim);

    if (*old_str != '\0') {
        *old_str++ = '\0';
    }

    if (saveptr) {
        *saveptr = old_str;
    }

    return res;
}

void *memccpy(void *dest, const void *src, int c, size_t n)
{
    size_t i;
    auto *d = reinterpret_cast<unsigned char *> (dest);
    const auto *s = reinterpret_cast<const unsigned char *> (src);

    if (n > 0) {
        for (i = 0; i < n; i++) {
            d[i] = s[i];

            if (s[i] == c) {
                return d + i + 1;
            }
        }
    }

    return nullptr;
}

char *strrev(char *str)
{
    if (str) {
        size_t len = strlen(str);

        if (len > 0) {
            size_t i, j;
            char c = 0;

            for (i = 0, j = len - 1; i < j; i++, j--) {
                c = str[i];
                str[i] = str[j];
                str[j] = c;
            }
        }
    }

    return str;
}

char *strupr(char *str)
{
    char *p = str;

    do {
        *p = islower(*p) ? toupper(*p) : *p;
    }
    while (*p++ != '\0');

    return str;
}

char *strlwr(char *str)
{
    char *p = str;

    do {
        *p = isupper(*p) ? tolower(*p) : *p;
    }
    while (*p++ != '\0');

    return str;
}

void *mempcpy(void *dest, const void *src, size_t n)
{
    return static_cast<char *>(memcpy(dest, src, n)) + n;
}

char *stpcpy(char *dest, const char *src)
{
    size_t src_len = strlen(src);
    memcpy(dest, src, src_len + 1);
    return dest + src_len;
}

__END_DECLS
__MAYBE_END_STD_NAMESPACE