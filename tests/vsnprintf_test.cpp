/*
 * Copyright (c) 2011 The tyndur Project. All rights reserved.
 *
 * This code is derived from software contributed to the tyndur Project
 * by Kevin Wolf.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// https://github.com/BartMassey/printf-tests/blob/master/sources/tests-libc-sprintf.c

#include <gtest/gtest.h>
#include <limits.h>
#include <cstdio>
#include "config.h"

namespace __STD_NAMESPACE
{
int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
int sprintf(char *buffer, const char *fmt, ...);
int snprintf(char *buffer, size_t size, const char *fmt, ...);
}

static constexpr const char *kNull = "(null)";

static constexpr const char *kNil = "(nil)";

#define BUFFER_SIZE 10000 * 2

#define DO_TEST(buf, ret, fmt, ...) do { \
        char lbuf[64] = { 0 }; \
        int lret = __STD_NAMESPACE::sprintf(lbuf, fmt, ## __VA_ARGS__); \
        ASSERT_EQ(ret, lret);                                     \
        ASSERT_STREQ(lbuf, buf);          \
    } while(0);

TEST(GCCLibCsnprintfTest, GenericTest)
{
    int failed = 0;

    /* Ein String ohne alles */
    DO_TEST("Hallo heimur", 12, "Hallo heimur")

    /* Einfache Konvertierungen */
    DO_TEST("Hallo heimur", 12, "%s", "Hallo heimur")
    DO_TEST("1024", 4, "%d", 1024)
    DO_TEST("-1024", 5, "%d", -1024)
    DO_TEST("1024", 4, "%i", 1024)
    DO_TEST("-1024", 5, "%i", -1024)
    DO_TEST("1024", 4, "%u", 1024u)
    DO_TEST("4294966272", 10, "%u", -1024u)
    DO_TEST("777", 3, "%o", 0777u)
    DO_TEST("37777777001", 11, "%o", -0777u)
    DO_TEST("1234abcd", 8, "%x", 0x1234abcdu)
    DO_TEST("edcb5433", 8, "%x", -0x1234abcdu)
    DO_TEST("1234ABCD", 8, "%X", 0x1234abcdu)
    DO_TEST("EDCB5433", 8, "%X", -0x1234abcdu)
    DO_TEST("x", 1, "%c", 'x')
    DO_TEST("%", 1, "%%")
    /* Mit %c kann man auch Nullbytes ausgeben */
    DO_TEST("\0", 1, "%c", '\0')

    /* Vorzeichen erzwingen (Flag +) */
    DO_TEST("Hallo heimur", 12, "%+s", "Hallo heimur")
    DO_TEST("+1024", 5, "%+d", 1024)
    DO_TEST("-1024", 5, "%+d", -1024)
    DO_TEST("+1024", 5, "%+i", 1024)
    DO_TEST("-1024", 5, "%+i", -1024)
    DO_TEST("1024", 4, "%+u", 1024u)
    DO_TEST("4294966272", 10, "%+u", -1024u)
    DO_TEST("777", 3, "%+o", 0777u)
    DO_TEST("37777777001", 11, "%+o", -0777u)
    DO_TEST("1234abcd", 8, "%+x", 0x1234abcdu)
    DO_TEST("edcb5433", 8, "%+x", -0x1234abcdu)
    DO_TEST("1234ABCD", 8, "%+X", 0x1234abcdu)
    DO_TEST("EDCB5433", 8, "%+X", -0x1234abcdu)
    DO_TEST("x", 1, "%+c", 'x')

    /* Vorzeichenplatzhalter erzwingen (Flag <space>) */
    DO_TEST("Hallo heimur", 12, "% s", "Hallo heimur")
    DO_TEST(" 1024", 5, "% d", 1024)
    DO_TEST("-1024", 5, "% d", -1024)
    DO_TEST(" 1024", 5, "% i", 1024)
    DO_TEST("-1024", 5, "% i", -1024)
    DO_TEST("1024", 4, "% u", 1024u)
    DO_TEST("4294966272", 10, "% u", -1024u)
    DO_TEST("777", 3, "% o", 0777u)
    DO_TEST("37777777001", 11, "% o", -0777u)
    DO_TEST("1234abcd", 8, "% x", 0x1234abcdu)
    DO_TEST("edcb5433", 8, "% x", -0x1234abcdu)
    DO_TEST("1234ABCD", 8, "% X", 0x1234abcdu)
    DO_TEST("EDCB5433", 8, "% X", -0x1234abcdu)
    DO_TEST("x", 1, "% c", 'x')

    /* Flag + hat Vorrang über <space> */
    DO_TEST("Hallo heimur", 12, "%+ s", "Hallo heimur")
    DO_TEST("+1024", 5, "%+ d", 1024)
    DO_TEST("-1024", 5, "%+ d", -1024)
    DO_TEST("+1024", 5, "%+ i", 1024)
    DO_TEST("-1024", 5, "%+ i", -1024)
    DO_TEST("1024", 4, "%+ u", 1024u)
    DO_TEST("4294966272", 10, "%+ u", -1024u)
    DO_TEST("777", 3, "%+ o", 0777u)
    DO_TEST("37777777001", 11, "%+ o", -0777u)
    DO_TEST("1234abcd", 8, "%+ x", 0x1234abcdu)
    DO_TEST("edcb5433", 8, "%+ x", -0x1234abcdu)
    DO_TEST("1234ABCD", 8, "%+ X", 0x1234abcdu)
    DO_TEST("EDCB5433", 8, "%+ X", -0x1234abcdu)
    DO_TEST("x", 1, "%+ c", 'x')

    /* Alternative Form */
    DO_TEST("0777", 4, "%#o", 0777u)
    DO_TEST("037777777001", 12, "%#o", -0777u)
    DO_TEST("0x1234abcd", 10, "%#x", 0x1234abcdu)
    DO_TEST("0xedcb5433", 10, "%#x", -0x1234abcdu)
    DO_TEST("0X1234ABCD", 10, "%#X", 0x1234abcdu)
    DO_TEST("0XEDCB5433", 10, "%#X", -0x1234abcdu)
    DO_TEST("0", 1, "%#o", 0u)
    DO_TEST("0", 1, "%#x", 0u)
    DO_TEST("0", 1, "%#X", 0u)

    /* Feldbreite: Kleiner als Ausgabe */
    //DO_TEST("Hallo heimur",   12, "%1s",      "Hallo heimur")
    DO_TEST("1024", 4, "%1d", 1024)
    DO_TEST("-1024", 5, "%1d", -1024)
    DO_TEST("1024", 4, "%1i", 1024)
    DO_TEST("-1024", 5, "%1i", -1024)
    DO_TEST("1024", 4, "%1u", 1024u)
    DO_TEST("4294966272", 10, "%1u", -1024u)
    DO_TEST("777", 3, "%1o", 0777u)
    DO_TEST("37777777001", 11, "%1o", -0777u)
    DO_TEST("1234abcd", 8, "%1x", 0x1234abcdu)
    DO_TEST("edcb5433", 8, "%1x", -0x1234abcdu)
    DO_TEST("1234ABCD", 8, "%1X", 0x1234abcdu)
    DO_TEST("EDCB5433", 8, "%1X", -0x1234abcdu)
    DO_TEST("x", 1, "%1c", 'x')

    /* Feldbreite: Größer als Ausgabe */
    //DO_TEST("               Hallo",  20, "%20s",      "Hallo")
    DO_TEST("                1024", 20, "%20d", 1024)
    DO_TEST("               -1024", 20, "%20d", -1024)
    DO_TEST("                1024", 20, "%20i", 1024)
    DO_TEST("               -1024", 20, "%20i", -1024)
    DO_TEST("                1024", 20, "%20u", 1024u)
    DO_TEST("          4294966272", 20, "%20u", -1024u)
    DO_TEST("                 777", 20, "%20o", 0777u)
    DO_TEST("         37777777001", 20, "%20o", -0777u)
    DO_TEST("            1234abcd", 20, "%20x", 0x1234abcdu)
    DO_TEST("            edcb5433", 20, "%20x", -0x1234abcdu)
    DO_TEST("            1234ABCD", 20, "%20X", 0x1234abcdu)
    DO_TEST("            EDCB5433", 20, "%20X", -0x1234abcdu)
    DO_TEST("                   x", 20, "%20c", 'x')

    /* Feldbreite: Linksbündig */
    DO_TEST("Hallo               ", 20, "%-20s", "Hallo")
    DO_TEST("1024                ", 20, "%-20d", 1024)
    DO_TEST("-1024               ", 20, "%-20d", -1024)
    DO_TEST("1024                ", 20, "%-20i", 1024)
    DO_TEST("-1024               ", 20, "%-20i", -1024)
    DO_TEST("1024                ", 20, "%-20u", 1024u)
    DO_TEST("4294966272          ", 20, "%-20u", -1024u)
    DO_TEST("777                 ", 20, "%-20o", 0777u)
    DO_TEST("37777777001         ", 20, "%-20o", -0777u)
    DO_TEST("1234abcd            ", 20, "%-20x", 0x1234abcdu)
    DO_TEST("edcb5433            ", 20, "%-20x", -0x1234abcdu)
    DO_TEST("1234ABCD            ", 20, "%-20X", 0x1234abcdu)
    DO_TEST("EDCB5433            ", 20, "%-20X", -0x1234abcdu)
    DO_TEST("x                   ", 20, "%-20c", 'x')

    /* Feldbreite: Padding mit 0 */
    DO_TEST("00000000000000001024", 20, "%020d", 1024)
    DO_TEST("-0000000000000001024", 20, "%020d", -1024)
    DO_TEST("00000000000000001024", 20, "%020i", 1024)
    DO_TEST("-0000000000000001024", 20, "%020i", -1024)
    DO_TEST("00000000000000001024", 20, "%020u", 1024u)
    DO_TEST("00000000004294966272", 20, "%020u", -1024u)
    DO_TEST("00000000000000000777", 20, "%020o", 0777u)
    DO_TEST("00000000037777777001", 20, "%020o", -0777u)

    DO_TEST("0000000000001234abcd", 20, "%020x", 0x1234abcdu)
    DO_TEST("000000000000edcb5433", 20, "%020x", -0x1234abcdu)
    DO_TEST("0000000000001234ABCD", 20, "%020X", 0x1234abcdu)
    DO_TEST("000000000000EDCB5433", 20, "%020X", -0x1234abcdu)

    /* Feldbreite: Padding und alternative Form */
    DO_TEST("                0777", 20, "%#20o", 0777u)
    DO_TEST("        037777777001", 20, "%#20o", -0777u)
    DO_TEST("          0x1234abcd", 20, "%#20x", 0x1234abcdu)
    DO_TEST("          0xedcb5433", 20, "%#20x", -0x1234abcdu)
    DO_TEST("          0X1234ABCD", 20, "%#20X", 0x1234abcdu)
    DO_TEST("          0XEDCB5433", 20, "%#20X", -0x1234abcdu)

    DO_TEST("00000000000000000777", 20, "%#020o", 0777u)
    DO_TEST("00000000037777777001", 20, "%#020o", -0777u)
    DO_TEST("0x00000000001234abcd", 20, "%#020x", 0x1234abcdu)
    DO_TEST("0x0000000000edcb5433", 20, "%#020x", -0x1234abcdu)
    DO_TEST("0X00000000001234ABCD", 20, "%#020X", 0x1234abcdu)
    DO_TEST("0X0000000000EDCB5433", 20, "%#020X", -0x1234abcdu)

    /* Feldbreite: - hat Vorrang vor 0 */
    DO_TEST("Hallo               ", 20, "%0-20s", "Hallo")
    DO_TEST("1024                ", 20, "%0-20d", 1024)
    DO_TEST("-1024               ", 20, "%0-20d", -1024)
    DO_TEST("1024                ", 20, "%0-20i", 1024)
    DO_TEST("-1024               ", 20, "%0-20i", -1024)
    DO_TEST("1024                ", 20, "%0-20u", 1024u)
    DO_TEST("4294966272          ", 20, "%0-20u", -1024u)
    DO_TEST("777                 ", 20, "%-020o", 0777u)
    DO_TEST("37777777001         ", 20, "%-020o", -0777u)
    DO_TEST("1234abcd            ", 20, "%-020x", 0x1234abcdu)
    DO_TEST("edcb5433            ", 20, "%-020x", -0x1234abcdu)
    DO_TEST("1234ABCD            ", 20, "%-020X", 0x1234abcdu)
    DO_TEST("EDCB5433            ", 20, "%-020X", -0x1234abcdu)
    DO_TEST("x                   ", 20, "%-020c", 'x')

    /* Feldbreite: Aus Parameter */
    DO_TEST("               Hallo", 20, "%*s", 20, "Hallo")
    DO_TEST("                1024", 20, "%*d", 20, 1024)
    DO_TEST("               -1024", 20, "%*d", 20, -1024)
    DO_TEST("                1024", 20, "%*i", 20, 1024)
    DO_TEST("               -1024", 20, "%*i", 20, -1024)
    DO_TEST("                1024", 20, "%*u", 20, 1024u)
    DO_TEST("          4294966272", 20, "%*u", 20, -1024u)
    DO_TEST("                 777", 20, "%*o", 20, 0777u)
    DO_TEST("         37777777001", 20, "%*o", 20, -0777u)
    DO_TEST("            1234abcd", 20, "%*x", 20, 0x1234abcdu)
    DO_TEST("            edcb5433", 20, "%*x", 20, -0x1234abcdu)
    DO_TEST("            1234ABCD", 20, "%*X", 20, 0x1234abcdu)
    DO_TEST("            EDCB5433", 20, "%*X", 20, -0x1234abcdu)
    DO_TEST("                   x", 20, "%*c", 20, 'x')

    /* Präzision / Mindestanzahl von Ziffern */
    DO_TEST("Hallo heimur", 12, "%.20s", "Hallo heimur")
    DO_TEST("00000000000000001024", 20, "%.20d", 1024)
    DO_TEST("-00000000000000001024", 21, "%.20d", -1024)
    DO_TEST("00000000000000001024", 20, "%.20i", 1024)
    DO_TEST("-00000000000000001024", 21, "%.20i", -1024)
    DO_TEST("00000000000000001024", 20, "%.20u", 1024u)
    DO_TEST("00000000004294966272", 20, "%.20u", -1024u)
    DO_TEST("00000000000000000777", 20, "%.20o", 0777u)
    DO_TEST("00000000037777777001", 20, "%.20o", -0777u)
    DO_TEST("0000000000001234abcd", 20, "%.20x", 0x1234abcdu)
    DO_TEST("000000000000edcb5433", 20, "%.20x", -0x1234abcdu)
    DO_TEST("0000000000001234ABCD", 20, "%.20X", 0x1234abcdu)
    DO_TEST("000000000000EDCB5433", 20, "%.20X", -0x1234abcdu)

    /* Feldbreite und Präzision */
    //DO_TEST("               Hallo",   20, "%20.5s",     "Hallo heimur")
    DO_TEST("               01024", 20, "%20.5d", 1024)
    DO_TEST("              -01024", 20, "%20.5d", -1024)
    DO_TEST("               01024", 20, "%20.5i", 1024)
    DO_TEST("              -01024", 20, "%20.5i", -1024)
    DO_TEST("               01024", 20, "%20.5u", 1024u)
    DO_TEST("          4294966272", 20, "%20.5u", -1024u)
    DO_TEST("               00777", 20, "%20.5o", 0777u)
    DO_TEST("         37777777001", 20, "%20.5o", -0777u)
    DO_TEST("            1234abcd", 20, "%20.5x", 0x1234abcdu)
    DO_TEST("          00edcb5433", 20, "%20.10x", -0x1234abcdu)
    DO_TEST("            1234ABCD", 20, "%20.5X", 0x1234abcdu)
    DO_TEST("          00EDCB5433", 20, "%20.10X", -0x1234abcdu)

    /* Präzision: 0 wird ignoriert */
    DO_TEST("               Hallo", 20, "%020.5s", "Hallo heimur")
    DO_TEST("               01024", 20, "%020.5d", 1024)
    DO_TEST("              -01024", 20, "%020.5d", -1024)
    DO_TEST("               01024", 20, "%020.5i", 1024)
    DO_TEST("              -01024", 20, "%020.5i", -1024)
    DO_TEST("               01024", 20, "%020.5u", 1024u)
    DO_TEST("          4294966272", 20, "%020.5u", -1024u)
    DO_TEST("               00777", 20, "%020.5o", 0777u)
    DO_TEST("         37777777001", 20, "%020.5o", -0777u)
    DO_TEST("            1234abcd", 20, "%020.5x", 0x1234abcdu)
    DO_TEST("          00edcb5433", 20, "%020.10x", -0x1234abcdu)
    DO_TEST("            1234ABCD", 20, "%020.5X", 0x1234abcdu)
    DO_TEST("          00EDCB5433", 20, "%020.10X", -0x1234abcdu)

    /* Präzision 0 */
    DO_TEST("", 0, "%.0s", "Hallo heimur")
    DO_TEST("                    ", 20, "%20.0s", "Hallo heimur")
    DO_TEST("", 0, "%.s", "Hallo heimur")
    DO_TEST("                    ", 20, "%20.s", "Hallo heimur")
    DO_TEST("                1024", 20, "%20.0d", 1024)
    DO_TEST("               -1024", 20, "%20.d", -1024)
    DO_TEST("                    ", 20, "%20.d", 0)
    DO_TEST("                1024", 20, "%20.0i", 1024)
    DO_TEST("               -1024", 20, "%20.i", -1024)
    DO_TEST("                    ", 20, "%20.i", 0)
    DO_TEST("                1024", 20, "%20.u", 1024u)
    DO_TEST("          4294966272", 20, "%20.0u", -1024u)
    DO_TEST("                    ", 20, "%20.u", 0u)

    DO_TEST("                 777", 20, "%20.o", 0777u)
    DO_TEST("         37777777001", 20, "%20.0o", -0777u)
    DO_TEST("                    ", 20, "%20.o", 0u)
    DO_TEST("            1234abcd", 20, "%20.x", 0x1234abcdu)
    DO_TEST("            edcb5433", 20, "%20.0x", -0x1234abcdu)
    DO_TEST("                    ", 20, "%20.x", 0u)
    DO_TEST("            1234ABCD", 20, "%20.X", 0x1234abcdu)
    DO_TEST("            EDCB5433", 20, "%20.0X", -0x1234abcdu)
    DO_TEST("                    ", 20, "%20.X", 0u)

    /* Negative Präzision wird ignoriert */
    /* XXX glibc tut nicht, was ich erwartet habe, vorerst deaktiviert... */

    DO_TEST("Hallo heimur", 12, "%.-42s", "Hallo heimur")
    DO_TEST("1024", 4, "%.-42d", 1024)
    DO_TEST("-1024", 5, "%.-42d", -1024)
    DO_TEST("1024", 4, "%.-42i", 1024)
    DO_TEST("-1024", 5, "%.-42i", -1024)
    DO_TEST("1024", 4, "%.-42u", 1024u)
    DO_TEST("4294966272", 10, "%.-42u", -1024u)
    DO_TEST("777", 3, "%.-42o", 0777u)
    DO_TEST("37777777001", 11, "%.-42o", -0777u)
    DO_TEST("1234abcd", 8, "%.-42x", 0x1234abcdu)
    DO_TEST("edcb5433", 8, "%.-42x", -0x1234abcdu)
    DO_TEST("1234ABCD", 8, "%.-42X", 0x1234abcdu)
    DO_TEST("EDCB5433", 8, "%.-42X", -0x1234abcdu)

    /*
     * Präzision und Feldbreite aus Parameter.
     * + hat Vorrang vor <space>, - hat Vorrang vor 0 (das eh ignoriert wird,
     * weil eine Präzision angegeben ist)
     */
    DO_TEST("Hallo               ", 20, "% -0+*.*s", 20, 5, "Hallo heimur")
    DO_TEST("+01024              ", 20, "% -0+*.*d", 20, 5, 1024)
    DO_TEST("-01024              ", 20, "% -0+*.*d", 20, 5, -1024)
    DO_TEST("+01024              ", 20, "% -0+*.*i", 20, 5, 1024)
    DO_TEST("-01024              ", 20, "% 0-+*.*i", 20, 5, -1024)
    DO_TEST("01024               ", 20, "% 0-+*.*u", 20, 5, 1024u)
    DO_TEST("4294966272          ", 20, "% 0-+*.*u", 20, 5, -1024u)
    DO_TEST("00777               ", 20, "%+ -0*.*o", 20, 5, 0777u)
    DO_TEST("37777777001         ", 20, "%+ -0*.*o", 20, 5, -0777u)
    DO_TEST("1234abcd            ", 20, "%+ -0*.*x", 20, 5, 0x1234abcdu)
    DO_TEST("00edcb5433          ", 20, "%+ -0*.*x", 20, 10, -0x1234abcdu)
    DO_TEST("1234ABCD            ", 20, "% -+0*.*X", 20, 5, 0x1234abcdu)
    DO_TEST("00EDCB5433          ", 20, "% -+0*.*X", 20, 10, -0x1234abcdu)

}

//===-- Unittests for snprintf --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// The sprintf test cases cover testing the shared printf functionality, so
// these tests will focus on snprintf exclusive features.

TEST(LlvmLibcSNPrintfTest, CutOff)
{
    char buff[64];
    int written;

    written =
        __STD_NAMESPACE::snprintf(buff, 16, "A simple string with no conversions.");
    EXPECT_EQ(written, 15);
    ASSERT_STREQ(buff, "A simple string");

    written = __STD_NAMESPACE::snprintf(buff, 5, "%s", "1234567890");
    EXPECT_EQ(written, 4);
    ASSERT_STREQ(buff, "1234");

    // passing null as the output pointer is allowed as long as buffsz is 0.
    written = __STD_NAMESPACE::snprintf(nullptr, 0, "%s and more", "1234567890");
    EXPECT_EQ(written, 0);
}

TEST(LlvmLibcSNPrintfTest, NoCutOff)
{
    char buff[64];
    int written;

    written =
        __STD_NAMESPACE::snprintf(buff, 37, "A simple string with no conversions.");
    EXPECT_EQ(written, 36);
    ASSERT_STREQ(buff, "A simple string with no conversions.");

    written = __STD_NAMESPACE::snprintf(buff, 20, "%s", "1234567890");
    EXPECT_EQ(written, 10);
    ASSERT_STREQ(buff, "1234567890");
}

TEST(LlvmLibcSPrintfTest, SimpleNoConv)
{
    char buff[64];
    int written;

    written = __STD_NAMESPACE::sprintf(buff, "A simple string with no conversions.");
    EXPECT_EQ(written, 36);
    ASSERT_STREQ(buff, "A simple string with no conversions.");
}

TEST(LlvmLibcSPrintfTest, PercentConv)
{
    char buff[64];
    int written;

    written = __STD_NAMESPACE::sprintf(buff, "%%");
    EXPECT_EQ(written, 1);
    ASSERT_STREQ(buff, "%");

    written = __STD_NAMESPACE::sprintf(buff, "abc %% def");
    EXPECT_EQ(written, 9);
    ASSERT_STREQ(buff, "abc % def");

    written = __STD_NAMESPACE::sprintf(buff, "%%%%%%");
    EXPECT_EQ(written, 3);
    ASSERT_STREQ(buff, "%%%");
}

TEST(LlvmLibcSPrintfTest, CharConv)
{
    char buff[64];
    int written;

    written = __STD_NAMESPACE::sprintf(buff, "%c", 'a');
    EXPECT_EQ(written, 1);
    ASSERT_STREQ(buff, "a");

    written = __STD_NAMESPACE::sprintf(buff, "%3c %-3c", '1', '2');
    EXPECT_EQ(written, 7);
    ASSERT_STREQ(buff, "  1 2  ");

    written = __STD_NAMESPACE::sprintf(buff, "%*c", 2, '3');
    EXPECT_EQ(written, 2);
    ASSERT_STREQ(buff, " 3");
}

TEST(LlvmLibcSPrintfTest, StringConv)
{
    char buff[64];
    int written;

    written = __STD_NAMESPACE::sprintf(buff, "%s", "abcDEF123");
    EXPECT_EQ(written, 9);
    ASSERT_STREQ(buff, "abcDEF123");

    written = __STD_NAMESPACE::sprintf(buff, "%10s %-10s", "centered", "title");
    EXPECT_EQ(written, 21);
    ASSERT_STREQ(buff, "  centered title     ");

    written = __STD_NAMESPACE::sprintf(buff, "%-5.4s%-4.4s", "words can describe",
                                       "soups most delicious");
    EXPECT_EQ(written, 9);
    ASSERT_STREQ(buff, "word soup");

    written = __STD_NAMESPACE::sprintf(buff, "%*s %.*s %*.*s", 10, "beginning", 2,
                                       "isn't", 12, 10, "important. Ever.");
    EXPECT_EQ(written, 26);
    ASSERT_STREQ(buff, " beginning is   important.");
}

TEST(LlvmLibcSPrintfTest, IntConv)
{
    char buff[64];
    int written;

    // Basic Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%d", 123);
    EXPECT_EQ(written, 3);
    ASSERT_STREQ(buff, "123");

    written = __STD_NAMESPACE::sprintf(buff, "%i", -456);
    EXPECT_EQ(written, 4);
    ASSERT_STREQ(buff, "-456");

    // Length Modifier Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%hhu", 257); // 0x101
    EXPECT_EQ(written, 1);
    ASSERT_STREQ(buff, "1");

    written = __STD_NAMESPACE::sprintf(buff, "%llu", 18446744073709551615ull);
    EXPECT_EQ(written, 20);
    ASSERT_STREQ(buff, "18446744073709551615"); // ull max

    written = __STD_NAMESPACE::sprintf(buff, "%tu", ~ptrdiff_t(0));
    if (sizeof(ptrdiff_t) == 8) {
        EXPECT_EQ(written, 20);
        ASSERT_STREQ(buff, "18446744073709551615");
    }
    else if (sizeof(ptrdiff_t) == 4) {
        EXPECT_EQ(written, 10);
        ASSERT_STREQ(buff, "4294967296");
    }

    written = __STD_NAMESPACE::sprintf(buff, "%lld", -9223372036854775807ll - 1ll);
    EXPECT_EQ(written, 20);
    ASSERT_STREQ(buff, "-9223372036854775808"); // ll min

    // Min Width Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%4d", 789);
    EXPECT_EQ(written, 4);
    ASSERT_STREQ(buff, " 789");

    written = __STD_NAMESPACE::sprintf(buff, "%2d", 987);
    EXPECT_EQ(written, 3);
    ASSERT_STREQ(buff, "987");

    // Precision Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%d", 0);
    EXPECT_EQ(written, 1);
    ASSERT_STREQ(buff, "0");

    written = __STD_NAMESPACE::sprintf(buff, "%.0d", 0);
    EXPECT_EQ(written, 0);
    ASSERT_STREQ(buff, "");

    written = __STD_NAMESPACE::sprintf(buff, "%.5d", 654);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "00654");

    written = __STD_NAMESPACE::sprintf(buff, "%.5d", -321);
    EXPECT_EQ(written, 6);
    ASSERT_STREQ(buff, "-00321");

    written = __STD_NAMESPACE::sprintf(buff, "%.2d", 135);
    EXPECT_EQ(written, 3);
    ASSERT_STREQ(buff, "135");

    // Flag Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%.5d", -321);
    EXPECT_EQ(written, 6);
    ASSERT_STREQ(buff, "-00321");

    written = __STD_NAMESPACE::sprintf(buff, "%-5d", 246);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "246  ");

    written = __STD_NAMESPACE::sprintf(buff, "%-5d", -147);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "-147 ");

    written = __STD_NAMESPACE::sprintf(buff, "%+d", 258);
    EXPECT_EQ(written, 4);
    ASSERT_STREQ(buff, "+258");

    written = __STD_NAMESPACE::sprintf(buff, "% d", 369);
    EXPECT_EQ(written, 4);
    ASSERT_STREQ(buff, " 369");

    written = __STD_NAMESPACE::sprintf(buff, "%05d", 470);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "00470");

    written = __STD_NAMESPACE::sprintf(buff, "%05d", -581);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "-0581");

    // Combined Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%+ u", 692);
    EXPECT_EQ(written, 3);
    ASSERT_STREQ(buff, "692");

    written = __STD_NAMESPACE::sprintf(buff, "%+ -05d", 703);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "+703 ");

    written = __STD_NAMESPACE::sprintf(buff, "%7.5d", 814);
    EXPECT_EQ(written, 7);
    ASSERT_STREQ(buff, "  00814");

    written = __STD_NAMESPACE::sprintf(buff, "%7.5d", -925);
    EXPECT_EQ(written, 7);
    ASSERT_STREQ(buff, " -00925");

    written = __STD_NAMESPACE::sprintf(buff, "%7.5d", 159);
    EXPECT_EQ(written, 7);
    ASSERT_STREQ(buff, "  00159");

    written = __STD_NAMESPACE::sprintf(buff, "% -7.5d", 260);
    EXPECT_EQ(written, 7);
    ASSERT_STREQ(buff, " 00260 ");

    written = __STD_NAMESPACE::sprintf(buff, "%5.4d", 10000);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "10000");

    // Multiple Conversion Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%10d %-10d", 456, -789);
    EXPECT_EQ(written, 21);
    ASSERT_STREQ(buff, "       456 -789      ");

    written = __STD_NAMESPACE::sprintf(buff, "%-5.4d%+.4u", 75, 25);
    EXPECT_EQ(written, 9);
    ASSERT_STREQ(buff, "0075 0025");

    written = __STD_NAMESPACE::sprintf(buff, "% 05hhi %+-0.5llu %-+ 06.3zd",
                                       256 + 127, 68719476736ll, size_t(2));
    EXPECT_EQ(written, 24);
    ASSERT_STREQ(buff, " 0127 68719476736 +002  ");
}

TEST(LlvmLibcSPrintfTest, HexConv)
{
    char buff[64];
    int written;

    // Basic Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%x", 0x123a);
    EXPECT_EQ(written, 4);
    ASSERT_STREQ(buff, "123a");

    written = __STD_NAMESPACE::sprintf(buff, "%X", 0x456b);
    EXPECT_EQ(written, 4);
    ASSERT_STREQ(buff, "456B");

    // Length Modifier Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%hhx", 0x10001);
    EXPECT_EQ(written, 1);
    ASSERT_STREQ(buff, "1");

    written = __STD_NAMESPACE::sprintf(buff, "%llx", 0xffffffffffffffffull);
    EXPECT_EQ(written, 16);
    ASSERT_STREQ(buff, "ffffffffffffffff"); // ull max

    written = __STD_NAMESPACE::sprintf(buff, "%tX", ~ptrdiff_t(0));
    if (sizeof(ptrdiff_t) == 8) {
        EXPECT_EQ(written, 16);
        ASSERT_STREQ(buff, "FFFFFFFFFFFFFFFF");
    }
    else if (sizeof(ptrdiff_t) == 4) {
        EXPECT_EQ(written, 8);
        ASSERT_STREQ(buff, "FFFFFFFF");
    }

    // Min Width Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%4x", 0x789);
    EXPECT_EQ(written, 4);
    ASSERT_STREQ(buff, " 789");

    written = __STD_NAMESPACE::sprintf(buff, "%2X", 0x987);
    EXPECT_EQ(written, 3);
    ASSERT_STREQ(buff, "987");

    // Precision Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%x", 0);
    EXPECT_EQ(written, 1);
    ASSERT_STREQ(buff, "0");

    written = __STD_NAMESPACE::sprintf(buff, "%.0x", 0);
    EXPECT_EQ(written, 0);
    ASSERT_STREQ(buff, "");

    written = __STD_NAMESPACE::sprintf(buff, "%.5x", 0x1F3);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "001f3");

    written = __STD_NAMESPACE::sprintf(buff, "%.2x", 0x135);
    EXPECT_EQ(written, 3);
    ASSERT_STREQ(buff, "135");

    // Flag Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%-5x", 0x246);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "246  ");

    written = __STD_NAMESPACE::sprintf(buff, "%#x", 0xd3f);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "0xd3f");

    written = __STD_NAMESPACE::sprintf(buff, "%#X", 0xE40);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "0XE40");

    written = __STD_NAMESPACE::sprintf(buff, "%05x", 0x470);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "00470");

    written = __STD_NAMESPACE::sprintf(buff, "%0#6x", 0x8c3);
    EXPECT_EQ(written, 6);
    ASSERT_STREQ(buff, "0x08c3");

    written = __STD_NAMESPACE::sprintf(buff, "%-#6x", 0x5f0);
    EXPECT_EQ(written, 6);
    ASSERT_STREQ(buff, "0x5f0 ");

    // Combined Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%#-07x", 0x703);
    EXPECT_EQ(written, 7);
    ASSERT_STREQ(buff, "0x703  ");

    written = __STD_NAMESPACE::sprintf(buff, "%7.5x", 0x814);
    EXPECT_EQ(written, 7);
    ASSERT_STREQ(buff, "  00814");

    written = __STD_NAMESPACE::sprintf(buff, "%#9.5X", 0x9d4);
    EXPECT_EQ(written, 9);
    ASSERT_STREQ(buff, "  0X009D4");

    written = __STD_NAMESPACE::sprintf(buff, "%-7.5x", 0x260);
    EXPECT_EQ(written, 7);
    ASSERT_STREQ(buff, "00260  ");

    written = __STD_NAMESPACE::sprintf(buff, "%5.4x", 0x10000);
    EXPECT_EQ(written, 5);
    ASSERT_STREQ(buff, "10000");

    // Multiple Conversion Tests.

    written = __STD_NAMESPACE::sprintf(buff, "%10X %-#10x", 0x45b, 0x789);
    EXPECT_EQ(written, 21);
    ASSERT_STREQ(buff, "       45B 0x789     ");

    written = __STD_NAMESPACE::sprintf(buff, "%-5.4x%#.4x", 0x75, 0x25);
    EXPECT_EQ(written, 11);
    ASSERT_STREQ(buff, "0075 0x0025");

    written = __STD_NAMESPACE::sprintf(buff, "%04hhX %#.5llx %-6.3zX", 256 + 0x7f,
                                       0x1000000000ll, size_t(2));
    EXPECT_EQ(written, 24);
    ASSERT_STREQ(buff, "007F 0x1000000000 002   ");
}

TEST(LlvmLibcSPrintfTest, PointerConv)
{
    char buff[64];
    int written;

    written = __STD_NAMESPACE::sprintf(buff, "%p", nullptr);
    EXPECT_EQ(written, /* 9 */ 5);
    ASSERT_STREQ(buff, /* "(nullptr)" */ "(nil)");

    written = __STD_NAMESPACE::sprintf(buff, "%p", 0x1a2b3c4d);
    EXPECT_EQ(written, 16);
    ASSERT_STREQ(buff, "000000001a2b3c4d");

    written = __STD_NAMESPACE::sprintf(buff, "%p", buff);
    EXPECT_GT(written, 0);
}

TEST(LlvmLibcSPrintfTest, WriteIntConv)
{
    char buff[64];
    int written;
    int test_val = -1;

    test_val = -1;
    written = __STD_NAMESPACE::sprintf(buff, "12345%n67890", &test_val);
    EXPECT_EQ(written, 10);
    EXPECT_EQ(test_val, 5);
    ASSERT_STREQ(buff, "1234567890");

    test_val = -1;
    written = __STD_NAMESPACE::sprintf(buff, "%n", &test_val);
    EXPECT_EQ(written, 0);
    EXPECT_EQ(test_val, 0);
    ASSERT_STREQ(buff, "");

#if 0 /* seems here expected some llvm specific behaviour */
    test_val = 0x100;
    written = __STD_NAMESPACE::sprintf(buff, "ABC%hhnDEF", &test_val);
    EXPECT_EQ(written, 6);
    EXPECT_EQ(test_val, 0x103);
    ASSERT_STREQ(buff, "ABCDEF");

    test_val = -1;
    written = __STD_NAMESPACE::sprintf(buff, "%s%n", "87654321", &test_val);
    EXPECT_EQ(written, 8);
    EXPECT_EQ(test_val, 8);
    ASSERT_STREQ(buff, "87654321");

    written = __STD_NAMESPACE::sprintf(buff, "abc123%n", nullptr);
    EXPECT_LT(written, 0);
#endif
}

TEST(LibcSnprintfTest, StringParameters)
{
    char buffer[BUFFER_SIZE];
    int res = __STD_NAMESPACE::snprintf(buffer, BUFFER_SIZE, "%s", "Hallo heimur");
    ASSERT_EQ(res, strlen("Hallo heimur"));
    ASSERT_STREQ("Hallo heimur", buffer);
}

TEST(LibcSnprintfTest, NullParameters)
{
    char buffer[BUFFER_SIZE];
    int res = __STD_NAMESPACE::snprintf(buffer, BUFFER_SIZE, "%s", NULL);
    ASSERT_EQ(res, strlen(kNull));
    ASSERT_STREQ("(null)", buffer);

    res = __STD_NAMESPACE::snprintf(buffer, BUFFER_SIZE, "%p", NULL);
    ASSERT_EQ(res, strlen(kNil));
    ASSERT_STREQ("(nil)", buffer);
}

TEST(LibcSnprintfTest, Range)
{
    char buffer[BUFFER_SIZE];
    size_t null_len = strlen(kNull);
    int res = -1;

    for (size_t i = 1; i < null_len; i++) {
        res = __STD_NAMESPACE::snprintf(buffer, i, "%s", NULL);

        if (i > 1) {
            ASSERT_EQ(res, i - 1);
            ASSERT_EQ(0, memcmp(buffer, kNull, i - 1));
        }
        else {
            ASSERT_EQ(res, 0);
        }
    }
}

TEST(LibcSnprintfTest, MinMax)
{
    char buffer[64];         // обычный буфер
    char small[10];          // маленький буфер для проверки усечения
    int ret;                 // возвращаемое значение __STD_NAMESPACE::snprintf

    // 1) INT_MIN и INT_MAX
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%d", INT_MIN);
    EXPECT_EQ(ret, static_cast<int>(std::to_string(INT_MIN).size()));
    EXPECT_STREQ(buffer, std::to_string(INT_MIN).c_str());

    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%d", INT_MAX);
    EXPECT_EQ(ret, static_cast<int>(std::to_string(INT_MAX).size()));
    EXPECT_STREQ(buffer, std::to_string(INT_MAX).c_str());

    // 2) LONG_MIN и LONG_MAX
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%ld", LONG_MIN);
    EXPECT_STREQ(buffer, std::to_string(LONG_MIN).c_str());

    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%ld", LONG_MAX);
    EXPECT_STREQ(buffer, std::to_string(LONG_MAX).c_str());

    // 3) LLONG_MIN и LLONG_MAX
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%lld", LLONG_MIN);
    EXPECT_STREQ(buffer, std::to_string(LLONG_MIN).c_str());

    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%lld", LLONG_MAX);
    EXPECT_STREQ(buffer, std::to_string(LLONG_MAX).c_str());

    // 4) unsigned: UINT_MAX, ULONG_MAX, ULLONG_MAX
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%u", UINT_MAX);
    EXPECT_STREQ(buffer, std::to_string(UINT_MAX).c_str());

    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%lu", ULONG_MAX);
    EXPECT_STREQ(buffer, std::to_string(ULONG_MAX).c_str());

    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%llu", ULLONG_MAX);
    EXPECT_STREQ(buffer, std::to_string(ULLONG_MAX).c_str());

    // 5) Несколько аргументов вместе
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer), "%d %ld %lld %u",
                                    INT_MIN, LONG_MAX, LLONG_MIN, UINT_MAX);
    ASSERT_GT(ret, 0);
    char expected[128];
    snprintf(expected, sizeof(expected), "%d %ld %lld %u",
             INT_MIN, LONG_MAX, LLONG_MIN, UINT_MAX);
    EXPECT_STREQ(buffer, expected);

    // 6) Буфер слишком мал (например, для INT_MIN нужно 11 байт с null, а буфер 10)
    ret = __STD_NAMESPACE::snprintf(small, sizeof(small), "%d", INT_MIN);
    EXPECT_EQ(ret, static_cast<int>(std::to_string(INT_MIN).size()));
    char truncated[10];
    snprintf(truncated, sizeof(truncated), "%d", INT_MIN);
    EXPECT_STREQ(small, truncated);
}

// Если тестируете свою реализацию __STD_NAMESPACE::snprintf
// Если стандартную — замените на snprintf

TEST(LibcSnprintfTest, StressTestUpTo50Simple)
{
    char buffer[4096];
    int ret;

    // 10 спецификаторов
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer),
                                    "%d %u %ld %lu %lld %llu %hd %hu %c %s",
                                    -123, 456U, 789L, 101112UL, -131415LL, 161718ULL,
                                    (short) -20000, (unsigned short) 60000, 'A', "Hello10");
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // 20 спецификаторов (добавляем ширину, но без *)
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer),
                                    "%10d %-10u %20ld %-20lu %15lld %-15llu %5hd %-5hu %c %s "
                                    "%08x %-12X %+#10o %#-8x %010u %+10d %-10ld %+20lld %#10x %s",
                                    -1, 2U, -3L, 4UL, -5LL, 6ULL,
                                    (short) 7, (unsigned short) 8, 'B', "Part1",
                                    0xABCD, 0xEF, 0777, 0x1234, 12345U, 67890,
                                    -11111L, -22222LL, 0xDEAD, "Part2");
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // 30 спецификаторов (комбинации с точностью, без *)
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer),
                                    "%d %.5d %-8.3d %10.6u %-15.4u %ld %.10ld %-20.8ld %lu %.5lu %-12.3lu "
                                    "%lld %.12lld %-18.6lld %llu %.8llu %-10.4llu %hd %.4hd %-6.2hd %hu %.5hu %-8.3hu "
                                    "%c %-5c %.3s %-10.6s %p",
                                    -100, 200, 300, 400U, 500U, -600L, 700L, 800L, 900UL, 1000UL, 1100UL,
                                    -1200LL, 1300LL, 1400LL, 1500ULL, 1600ULL, 1700ULL,
                                    (short) -1800, (short) 1900, (short) 2000,
                                    (unsigned short) 2100, (unsigned short) 2200, (unsigned short) 2300,
                                    'X', 'Y', "Hello", "World", (void *) 0x7FFF);
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // 40 спецификаторов (повторяем безопасный паттерн без * и сложных unsigned отрицательных)
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer),
                                    "%d %d %d %d %u %u %u %u %ld %ld %ld %ld %lu %lu %lu %lu "
                                    "%lld %lld %lld %lld %llu %llu %llu %llu %hd %hd %hd %hd %hu %hu %hu %hu "
                                    "%c %c %c %c %s %s %s %s",
                                    1, 2, 3, 4, 5U, 6U, 7U, 8U,
                                    10L, 20L, 30L, 40L, 50UL, 60UL, 70UL, 80UL,
                                    100LL, 200LL, 300LL, 400LL, 500ULL, 600ULL, 700ULL, 800ULL,
                                    (short) 11, (short) 12, (short) 13, (short) 14,
                                    (unsigned short) 21, (unsigned short) 22, (unsigned short) 23, (unsigned short) 24,
                                    'a', 'b', 'c', 'd', "s1", "s2", "s3", "s4");
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // 50 спецификаторов (максимальный стресс, базовые типы)
    ret = __STD_NAMESPACE::snprintf(buffer,
                                    sizeof(buffer),
                                    "%d %d %d %d %d %u %u %u %u %u "
                                    "%ld %ld %ld %ld %ld %lu %lu %lu %lu %lu "
                                    "%lld %lld %lld %lld %lld %llu %llu %llu %llu %llu "
                                    "%hd %hd %hd %hd %hd %hu %hu %hu %hu %hu "
                                    "%c %c %c %c %c %s %s %s %s %s",
                                    1,
                                    2,
                                    3,
                                    4,
                                    5,
                                    6U,
                                    7U,
                                    8U,
                                    9U,
                                    10U,
                                    11L,
                                    12L,
                                    13L,
                                    14L,
                                    15L,
                                    16UL,
                                    17UL,
                                    18UL,
                                    19UL,
                                    20UL,
                                    21LL,
                                    22LL,
                                    23LL,
                                    24LL,
                                    25LL,
                                    26ULL,
                                    27ULL,
                                    28ULL,
                                    29ULL,
                                    30ULL,
                                    (short) 31,
                                    (short) 32,
                                    (short) 33,
                                    (short) 34,
                                    (short) 35,
                                    (unsigned short) 36,
                                    (unsigned short) 37,
                                    (unsigned short) 38,
                                    (unsigned short) 39,
                                    (unsigned short) 40,
                                    'A',
                                    'B',
                                    'C',
                                    'D',
                                    'E',
                                    "s1",
                                    "s2",
                                    "s3",
                                    "s4",
                                    "s5");
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // Дополнительно: проверка с очень маленьким буфером (для последнего вызова)
    char small[10];
    ret = __STD_NAMESPACE::snprintf(small,
                                    sizeof(small),
                                    "%d %d %d %d %d %u %u %u %u %u "
                                    "%ld %ld %ld %ld %ld %lu %lu %lu %lu %lu "
                                    "%lld %lld %lld %lld %lld %llu %llu %llu %llu %llu "
                                    "%hd %hd %hd %hd %hd %hu %hu %hu %hu %hu "
                                    "%c %c %c %c %c %s %s %s %s %s",
                                    1,
                                    2,
                                    3,
                                    4,
                                    5,
                                    6U,
                                    7U,
                                    8U,
                                    9U,
                                    10U,
                                    11L,
                                    12L,
                                    13L,
                                    14L,
                                    15L,
                                    16UL,
                                    17UL,
                                    18UL,
                                    19UL,
                                    20UL,
                                    21LL,
                                    22LL,
                                    23LL,
                                    24LL,
                                    25LL,
                                    26ULL,
                                    27ULL,
                                    28ULL,
                                    29ULL,
                                    30ULL,
                                    (short) 31,
                                    (short) 32,
                                    (short) 33,
                                    (short) 34,
                                    (short) 35,
                                    (unsigned short) 36,
                                    (unsigned short) 37,
                                    (unsigned short) 38,
                                    (unsigned short) 39,
                                    (unsigned short) 40,
                                    'A',
                                    'B',
                                    'C',
                                    'D',
                                    'E',
                                    "s1",
                                    "s2",
                                    "s3",
                                    "s4",
                                    "s5");
    EXPECT_GT(ret, 0);  // должно потребоваться много места
    EXPECT_EQ(static_cast<size_t>(ret), sizeof(small) - 1);
    EXPECT_EQ(small[sizeof(small) - 1], '\0');  // всегда нуль-терминатор
}

// Если нужно тестировать std::snprintf, замените __STD_NAMESPACE::snprintf на snprintf

TEST(LibcSnprintfTest, StressTestSimpleFixed)
{
    char buffer[4096];
    int ret;

    // ---- 10 спецификаторов с фиксированной шириной/точностью ----
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer),
                                    "%10d %-10d %+10d %05d %-5d %08u %-8u %10.5s %-10.3s %p",
                                    -12345, 67890, 11111, 22222, 33333,
                                    44444U, 55555U, "HelloWorld", "HelloWorld", (void *) 0x7FFF);
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // ---- 20 спецификаторов (только целые и unsigned) ----
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer),
                                    "%d %5d %-8d %+10d %05d %-6d %08d %-10d %+05d %d "
                                    "%u %10u %-12u %08u %-5u %6u %-8u %+10u %05u %u",
                                    1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                                    11U, 12U, 13U, 14U, 15U, 16U, 17U, 18U, 19U, 20U);
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // ---- 30 спецификаторов (long, unsigned long, строки, символы) ----
    ret = __STD_NAMESPACE::snprintf(buffer,
                                    sizeof(buffer),
                                    "%ld %10ld %-10ld %+15ld %05ld %-8ld %20ld %-20ld %+10ld %ld "
                                    "%lu %15lu %-15lu %010lu %-10lu %20lu %-20lu %+10lu %05lu %lu "
                                    "%c %-5c %10c %-10c %s %20s %-20s %10.5s %-10.3s %p",
                                    1L,
                                    2L,
                                    3L,
                                    4L,
                                    5L,
                                    6L,
                                    7L,
                                    8L,
                                    9L,
                                    10L,
                                    11UL,
                                    12UL,
                                    13UL,
                                    14UL,
                                    15UL,
                                    16UL,
                                    17UL,
                                    18UL,
                                    19UL,
                                    20UL,
                                    'A',
                                    'B',
                                    'C',
                                    'D',
                                    "Short",
                                    "LongString",
                                    "Padding",
                                    "HelloWorld",
                                    "HelloWorld",
                                    (void *) 0x1234);
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // ---- 40 спецификаторов (long long, unsigned long long, short) ----
    ret = __STD_NAMESPACE::snprintf(buffer, sizeof(buffer),
                                    "%lld %20lld %-20lld %+15lld %010lld %-10lld %30lld %-30lld %+5lld %lld "
                                    "%llu %25llu %-25llu %020llu %-15llu %35llu %-35llu %+10llu %5llu %llu "
                                    "%hd %10hd %-10hd %+8hd %05hd %-6hd %hu %15hu %-15hu %hu",
                                    1LL, 2LL, 3LL, 4LL, 5LL, 6LL, 7LL, 8LL, 9LL, 10LL,
                                    11ULL, 12ULL, 13ULL, 14ULL, 15ULL, 16ULL, 17ULL, 18ULL, 19ULL, 20ULL,
                                    (short) 1, (short) 2, (short) 3, (short) 4, (short) 5, (short) 6,
                                    (unsigned short) 7, (unsigned short) 8, (unsigned short) 9, (unsigned short) 10);
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // ---- 50 спецификаторов (комбинация всех типов без *-флагов) ----
    ret = __STD_NAMESPACE::snprintf(buffer,
                                    sizeof(buffer),
                                    "%d %5d %-8d %+10d %05d %-6d %08d %-10d %+05d %d "
                                    "%u %10u %-12u %08u %-5u %6u %-8u %+10u %05u %u "
                                    "%ld %10ld %-10ld %+15ld %05ld %-8ld %20ld %-20ld %+10ld %ld "
                                    "%lu %15lu %-15lu %010lu %-10lu %20lu %-20lu %+10lu %05lu %lu "
                                    "%c %-5c %10c %-10c %s %20s %-20s %10.5s %-10.3s %p",
                                    1,
                                    2,
                                    3,
                                    4,
                                    5,
                                    6,
                                    7,
                                    8,
                                    9,
                                    10,
                                    11U,
                                    12U,
                                    13U,
                                    14U,
                                    15U,
                                    16U,
                                    17U,
                                    18U,
                                    19U,
                                    20U,
                                    21L,
                                    22L,
                                    23L,
                                    24L,
                                    25L,
                                    26L,
                                    27L,
                                    28L,
                                    29L,
                                    30L,
                                    31UL,
                                    32UL,
                                    33UL,
                                    34UL,
                                    35UL,
                                    36UL,
                                    37UL,
                                    38UL,
                                    39UL,
                                    40UL,
                                    'A',
                                    'B',
                                    'C',
                                    'D',
                                    "Short",
                                    "LongString",
                                    "Padding",
                                    "HelloWorld",
                                    "HelloWorld",
                                    (void *) 0xCAFE);
    EXPECT_GE(ret, 0);
    EXPECT_LT(static_cast<size_t>(ret), sizeof(buffer));

    // ---- Дополнительно: проверка усечения на маленьком буфере (для последней строки) ----
    char small[10];
    ret = __STD_NAMESPACE::snprintf(small, sizeof(small), "%s %s %d %d %d %d %d %d %d %d",
                                    "One", "Two", 1, 2, 3, 4, 5, 6, 7, 8);
    EXPECT_GT(ret, 0);
    EXPECT_EQ(static_cast<size_t>(ret), sizeof(small) - 1);
    EXPECT_EQ(small[sizeof(small) - 1], '\0');
}