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

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include "cstring.h"
#include "utils.h"

#define MAX_NUMBER_LEN 66

__MAYBE_BEGIN_STD_NAMESPACE

static constexpr const char *kNull = "(null)";

static constexpr const char *kNil = "(nil)";

static constexpr size_t kPointerHexDigits = sizeof(void *) * 2;

enum class LengthSpec: int
{
    null,
    h,
    hh,
    l,
    ll,
    z,
    t,
    ptr
};

enum FormatFlags: int
{
    F_NONE = 0,
    F_MINUS = 1 << 0,
    F_PLUS = 1 << 1,
    F_SPACE = 1 << 2,
    F_ALT = 1 << 3,
    F_ZERO = 1 << 4,
    F_HUGE = 1 << 5,
};

struct FormatSpec
{
    int flags{FormatFlags::F_NONE};
    LengthSpec lengthSpec{LengthSpec::null};
    int total{0};
    int field_width{-1};
    int precision{-1};
    bool negative{false};
    int radix{10};

    inline void reset() noexcept
    {
        flags = FormatFlags::F_NONE;
        lengthSpec = LengthSpec::null;
        field_width = -1;
        precision = -1;
        negative = false;
        radix = 10;
    }
};

struct va_list_wrapper
{
    va_list ap;
};

static const char *parse_number(const char *str, int &out)
{
    int v = 0;
    while (isdigit(*str)) {
        v = v * 10 + (*str - '0');
        ++str;
    }
    out = v;
    return str;
}

static const char *parse_flags_fw_precision(const char *fmt, FormatSpec &formatSpec, va_list_wrapper &w)
{
    do {
        switch (*fmt) {
            case '#':
                formatSpec.flags |= FormatFlags::F_ALT;
                continue;

            case ' ':
                formatSpec.flags |= FormatFlags::F_SPACE;
                continue;

            case '0':
                formatSpec.flags |= FormatFlags::F_ZERO;
                continue;

            case '-':
                formatSpec.flags |= FormatFlags::F_MINUS;
                continue;

            case '+':
                formatSpec.flags |= FormatFlags::F_PLUS;
                continue;

            default:
                break;
        }

        break;
    }
    while (*fmt++);

    if (*fmt == '*') {
        ++fmt;
        formatSpec.field_width = va_arg(w.ap, int);
    }
    else if (isdigit(*fmt)) {
        fmt = parse_number(fmt, formatSpec.field_width);
    }
    else if (*fmt == '-') {
        formatSpec.flags |= FormatFlags::F_MINUS;

        ++fmt;

        if (isdigit(*++fmt)) {
            fmt = parse_number(fmt, formatSpec.field_width);
        }
    }

    if (*fmt == '.') {
        formatSpec.precision = 0;
        formatSpec.flags &= ~FormatFlags::F_ZERO;
        ++fmt;
        if (*fmt == '*') {
            ++fmt;
            formatSpec.precision = va_arg(w.ap, int);
        }
        else if (*fmt == '-') {
            while (isdigit(*++fmt)) {}
            formatSpec.precision = -1;
        }
        else if (isdigit(*fmt)) {
            fmt = parse_number(fmt, formatSpec.precision);
        }
    }

    return fmt;
}

static const char *parse_size_specifier(const char *fmt, FormatSpec &formatSpec)
{
    switch (*fmt) {
        case 'h':
            formatSpec.lengthSpec = LengthSpec::h;

            if (*(fmt + 1) == 'h') {
                formatSpec.lengthSpec = LengthSpec::hh;
                ++fmt;
            }
            ++fmt;
            break;

        case 'l':
            formatSpec.lengthSpec = LengthSpec::l;

            if (*(fmt + 1) == 'l') {
                formatSpec.lengthSpec = LengthSpec::ll;
                ++fmt;
            }
            ++fmt;
            break;

        case 'z':
            formatSpec.lengthSpec = LengthSpec::z;
            ++fmt;
            break;

        case 't':
            formatSpec.lengthSpec = LengthSpec::t;
            ++fmt;
            break;
    }

    return fmt;
}

static char *
put_with_field_width(char *buf, const char *value, int len, FormatSpec &formatSpec, int size)
{
    int padding_len = formatSpec.field_width - len >= 0 ? formatSpec.field_width - len : 0;
    formatSpec.total += padding_len;

    if (!(formatSpec.flags & FormatFlags::F_MINUS)) {
        memset(buf, ' ', min(padding_len, size));
        buf += padding_len;
        size = size > padding_len ? size - padding_len : 0;
        padding_len = 0;
    }

    buf = static_cast<char *>(mempcpy(buf, value, min(len, size)));
    formatSpec.total += len;

    if (formatSpec.flags & FormatFlags::F_MINUS) {
        memset(buf, ' ', min(padding_len, size));
        buf += padding_len;
        padding_len = 0;
    }

    return buf;
}

template <typename T, typename U>
T read_from_va_list(va_list_wrapper &w)
{
    if constexpr (__is_same(U, char)) {
        const int value = va_arg(w.ap, int);
        return static_cast<T>(static_cast<char>(value));
    } else if constexpr (__is_same(U, signed char)) {
        const int value = va_arg(w.ap, int);
        return static_cast<T>(static_cast<signed char>(value));
    } else if constexpr (__is_same(U, unsigned char)) {
        const int value = va_arg(w.ap, int);
        return static_cast<T>(static_cast<unsigned char>(value));
    } else if constexpr (__is_same(U, short)) {
        const int value = va_arg(w.ap, int);
        return static_cast<T>(static_cast<short>(value));
    } else if constexpr (__is_same(U, unsigned short)) {
        const int value = va_arg(w.ap, int);
        return static_cast<T>(static_cast<unsigned short>(value));
    } else if constexpr (__is_same(U, void *)) {
        void *value = va_arg(w.ap, void *);
        return static_cast<T>(reinterpret_cast<uintptr_t>(value));
    } else {
        U value = va_arg(w.ap, U);
        return static_cast<T>(value);
    }
}

template <typename T> unsigned long long read_from_va_list_ull(va_list_wrapper &w)
{
    return read_from_va_list<unsigned long long, T>(w);
}

template <typename T> long long read_from_va_list_ll(va_list_wrapper &w)
{
    return read_from_va_list<long long, T>(w);
}

static char *format_char(char *buf, FormatSpec &formatSpec, int size, va_list_wrapper &w)
{
    char c = read_from_va_list<char, char>(w);
    return put_with_field_width(buf, &c, 1, formatSpec, size);
}

static char *format_string(char *buf, FormatSpec &formatSpec, int size, va_list_wrapper &w, const char *s = nullptr)
{
    auto str = s ? s : va_arg(w.ap, const char*);

    if (!str) {
        str = kNull;
    }

    int max_count = size;

    if (formatSpec.precision >= 0) {
        max_count = min(formatSpec.precision, size);
    }

    // I'm pretty sure the length of str is < INT_MAX :-)
    int len = static_cast<int>(strnlen(str, max_count));
    return put_with_field_width(buf, str, len, formatSpec, size);
}

static long long read_signed_number(const FormatSpec &formatSpec, va_list_wrapper &w)
{
    long long value = 0;

    switch (formatSpec.lengthSpec) {
        case LengthSpec::l:
            value = read_from_va_list_ll<long>(w);
            break;

        case LengthSpec::z:
            value = read_from_va_list_ll<int64_t>(w);
            break;

        case LengthSpec::hh:
            value = read_from_va_list_ll<signed char>(w);;
            break;

        case LengthSpec::t:
            value = read_from_va_list_ll<ptrdiff_t>(w);
            break;

        case LengthSpec::h:
            value = read_from_va_list_ll<int>(w);
            break;

        case LengthSpec::ll:
        default:
            value = read_from_va_list_ll<long long>(w);
            break;
    }

    return value;
}

static unsigned long long read_unsigned_number(const FormatSpec &formatSpec, va_list_wrapper &w)
{
    unsigned long long value = 0;

    switch (formatSpec.lengthSpec) {
        case LengthSpec::l:
            value = read_from_va_list_ull<unsigned long>(w);
            break;

        case LengthSpec::z:
            value = read_from_va_list_ull<size_t>(w);
            break;

        case LengthSpec::ptr:
            value = read_from_va_list_ull<void*>(w);
            break;

        case LengthSpec::hh:
            value = read_from_va_list_ull<unsigned char>(w);
            break;

        case LengthSpec::h:
            value = read_from_va_list_ull<unsigned int>(w);
            break;

        case LengthSpec::ll:
        default:
            value = read_from_va_list_ull<unsigned long long>(w);
            break;
    }

    return value;
}

int
number_to_string(char *buf, unsigned long long number, const FormatSpec &formatSpec, int size)
{
    const char *digits = (formatSpec.flags & FormatFlags::F_HUGE) > 0 ? "0123456789ABCDEF" : "0123456789abcdef";
    char num_str[MAX_NUMBER_LEN];
    int number_len = 0;
    int p = formatSpec.precision;
    int zero_count = 0;
    auto nr = number;

    if (formatSpec.precision == 0 && number == 0) {
        return 0;
    }

    if (nr != 0) {
        while (nr != 0) {
            unsigned long long n = nr % formatSpec.radix;
            nr /= formatSpec.radix;
            num_str[number_len++] = digits[n];
        }
    }
    else {
        num_str[number_len++] = '0';
    }

    p -= number_len;

    if (formatSpec.flags & FormatFlags::F_ZERO) {
        if (formatSpec.flags & FormatFlags::F_ALT) {
            if (number > 0) {
                switch (formatSpec.radix) {
                    case 8:
                        --p;
                        break;

                    case 16:
                        p -= 2;
                        break;
                }
            }
        }
    }

    zero_count = min(p, size);

    while (zero_count > 0) {
        num_str[number_len++] = '0';
        --zero_count;
    }

    if (formatSpec.flags & FormatFlags::F_ALT) {
        if (number > 0) {
            switch (formatSpec.radix) {
                case 8:
                    num_str[number_len++] = '0';
                    break;

                case 16:
                    num_str[number_len++] = (formatSpec.flags & FormatFlags::F_HUGE) > 0 ? 'X' : 'x';
                    num_str[number_len++] = '0';
                    break;
            }
        }
    }

    number_len = min(number_len, size);

    for (int i = 0; i < number_len; ++i) {
        *buf++ = num_str[number_len - i - 1];
    }

    return number_len;
}

static char *format_number(char *buf, unsigned long long number, FormatSpec &formatSpec, int size)
{
    char sign = formatSpec.negative ? '-' : 0;
    int number_len = 0;
    int offset = 0;
    char number_str[MAX_NUMBER_LEN];

    if (!formatSpec.negative) {
        if ((formatSpec.flags & FormatFlags::F_PLUS)) {
            sign = '+';
        }
        else if ((formatSpec.flags & FormatFlags::F_SPACE)) {
            sign = ' ';
        }
    }

    // emulate zero flag via precision
    if (formatSpec.flags & FormatFlags::F_ZERO) {
        if (formatSpec.precision < formatSpec.field_width) {
            formatSpec.precision = formatSpec.field_width;
            formatSpec.field_width = 0;
        }

        if (sign > 0) {
            --formatSpec.precision;
        }
    }

    offset = sign == 0 ? offset : offset + 1;
    number_str[0] = sign;
    number_len = number_to_string(&number_str[offset],
                                  number,
                                  formatSpec,
                                  MAX_NUMBER_LEN);
    number_len += offset;
    return put_with_field_width(buf, number_str, number_len, formatSpec, size);
}

static char *read_format_unsigned_int(char *buf, FormatSpec &formatSpec, int size, va_list_wrapper &w)
{
    formatSpec.flags &= ~FormatFlags::F_SPACE;
    formatSpec.flags &= ~FormatFlags::F_PLUS;
    auto value =
        formatSpec.lengthSpec == LengthSpec::null ? va_arg(w.ap, unsigned int) : read_unsigned_number(
            formatSpec,
            w);

    return format_number(buf, value, formatSpec, size);
}

__BEGIN_DECLS

int vsnprintf(char *buf, size_t max_size, const char *fmt, va_list ap)
{
    FormatSpec formatSpec;
    va_list_wrapper wrapper{};

    if (max_size == 0) {
        return 0;
    }

    va_copy(wrapper.ap, ap);

    if (max_size > 0 && max_size < static_cast<size_t>(INT_MAX)) {
        char *pos = buf;
        int size = static_cast<int>(max_size - 1);
        ptrdiff_t written = 0;
        // %[flags][field_width][.precision][size specifier]specifier
        for (int i = 0; *fmt && written < size; ++i, ++fmt) {
            formatSpec.reset();

            if (*fmt != '%') {
                *pos++ = *fmt;
                ++formatSpec.total;
                ++written;
                continue;
            }

            ++fmt;
            fmt = parse_flags_fw_precision(fmt, formatSpec, wrapper);
            fmt = parse_size_specifier(fmt, formatSpec);

            if (formatSpec.flags & FormatFlags::F_MINUS) {
                formatSpec.flags &= ~FormatFlags::F_ZERO;
            }

            switch (*fmt) {
                case '%':
                    *pos++ = *fmt;
                    ++formatSpec.total;
                    break;

                case 'd':
                case 'i': {
                    formatSpec.flags &= ~FormatFlags::F_ALT;
                    formatSpec.radix = 10;
                    auto value =
                        formatSpec.lengthSpec == LengthSpec::null ? va_arg(wrapper.ap, int) : read_signed_number(
                            formatSpec,
                            wrapper);
                    formatSpec.negative = value < 0;

                    if (value < 0) {
                        value = -value;
                        formatSpec.flags &= ~FormatFlags::F_SPACE;
                    }

                    pos = format_number(pos, value, formatSpec, size - written);

                }
                    break;

                case 'u': {
                    formatSpec.flags &= ~FormatFlags::F_ALT;
                    pos = read_format_unsigned_int(pos, formatSpec, size - written, wrapper);
                }
                    break;

                case 'o': {
                    formatSpec.radix = 8;
                    pos = read_format_unsigned_int(pos, formatSpec, size - written, wrapper);

                }
                    break;

                case 'X':
                    formatSpec.flags |= FormatFlags::F_HUGE;
                case 'x': {
                    {
                        formatSpec.radix = 16;
                        pos = read_format_unsigned_int(pos, formatSpec, size - written, wrapper);
                    }
                }
                    break;

                case 'p': {
                    formatSpec.lengthSpec = LengthSpec::ptr;
                    auto value = read_unsigned_number(formatSpec, wrapper);

                    if (value > 0) {
                        formatSpec.flags &= ~FormatFlags::F_ZERO;
                        formatSpec.flags &= ~FormatFlags::F_SPACE;
                        formatSpec.flags &= ~FormatFlags::F_PLUS;
                        formatSpec.flags &= ~FormatFlags::F_ALT;
                        formatSpec.radix = 16;

                        if (formatSpec.precision == -1 && formatSpec.field_width == -1) {
                            formatSpec.precision = kPointerHexDigits;
                        }

                        pos = format_number(pos, value, formatSpec, size - written);
                    }
                    else {
                        pos = format_string(pos, formatSpec, size - i, wrapper, kNil);
                    }
                }
                    break;

                case 'n': {
                    size_t *ptr = va_arg(wrapper.ap, size_t*);

                    if (ptr) {
                        *ptr = written;
                    }
                }
                    break;

                case 'c':
                    pos = format_char(pos, formatSpec, size - written, wrapper);
                    break;

                case 's':
                    pos = format_string(pos, formatSpec, size - written, wrapper);
                    break;

                default:
                    *pos++ = *fmt;
                    ++formatSpec.total;
                    break;
            }

            written = pos - buf;
        }

        *pos = '\0';
    }

    return formatSpec.total;
}

int sprintf(char *buffer, const char *fmt, ...)
{
    int res = -1;
    va_list ap;
    va_start(ap, fmt);
    res = vsnprintf(buffer, 1024, fmt, ap);
    va_end(ap);
    return res;
}

int snprintf(char *buffer, size_t size, const char *fmt, ...)
{
    int res = -1;
    va_list ap;
    va_start(ap, fmt);
    res = vsnprintf(buffer, size, fmt, ap);
    va_end(ap);
    return res;
}

__END_DECLS
__MAYBE_END_STD_NAMESPACE