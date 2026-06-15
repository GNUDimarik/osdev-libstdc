//===-- Unittests for isalnum----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "cctype.h"

int call_isalnum(int c)
{ return __STD_NAMESPACE::isalnum(c); }
int call_isascii(int c)
{ return __STD_NAMESPACE::isascii(c); }
int call_isalpha(int c)
{ return __STD_NAMESPACE::isalpha(c); }
int call_isblank(int c)
{ return __STD_NAMESPACE::isblank(c); }
int call_iscntrl(int c)
{ return __STD_NAMESPACE::iscntrl(c); }
int call_isdigit(int c)
{ return __STD_NAMESPACE::isdigit(c); }
int call_isgraph(int c)
{ return __STD_NAMESPACE::isgraph(c); }
int call_islower(int c)
{ return __STD_NAMESPACE::islower(c); }
int call_isprint(int c)
{ return __STD_NAMESPACE::isprint(c); }
int call_ispunct(int c)
{ return __STD_NAMESPACE::ispunct(c); }
int call_isspace(int c)
{ return __STD_NAMESPACE::isspace(c); }
int call_isupper(int c)
{ return __STD_NAMESPACE::isupper(c); }
int call_isxdigit(int c)
{ return __STD_NAMESPACE::isxdigit(c); }
int call_toascii(int c)
{ return __STD_NAMESPACE::toascii(c); }
int call_tolower(int c)
{ return __STD_NAMESPACE::tolower(c); }
int call_toupper(int c)
{ return __STD_NAMESPACE::toupper(c); }

#include <gtest/gtest.h>

//===-- Unittests for isalnum --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsAlNum, DefaultLocale)
{
    // Loops through all characters, verifying that numbers and letters
    // return non-zero integer and everything else returns a zero.
    for (int c = 0; c < 255; ++c) {
        if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ||
            ('0' <= c && c <= '9'))
            EXPECT_NE(call_isalnum(c), 0);
        else
            EXPECT_EQ(call_isalnum(c), 0);
    }
}

//===-- Unittests for isalpha --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsAlpha, DefaultLocale)
{
    // Loops through all characters, verifying that letters return a
    // non-zero integer and everything else returns zero.
    for (int ch = 0; ch < 255; ++ch) {
        if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
            EXPECT_NE(call_isalpha(ch), 0);
        else
            EXPECT_EQ(call_isalpha(ch), 0);
    }
}

//===-- Unittests for isascii --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsAscii, DefaultLocale)
{
    // Loops through all characters, verifying that ascii characters
    //    (which are all 7 bit unsigned integers)
    // return a non-zero integer and everything else returns zero.
    for (int ch = 0; ch < 255; ++ch) {
        if (ch <= 0x7f)
            EXPECT_NE(call_isascii(ch), 0);
        else
            EXPECT_EQ(call_isascii(ch), 0);
    }
}

//===-- Unittests for isblank --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsBlank, DefaultLocale)
{
    // Loops through all characters, verifying that space and horizontal tab
    // return a non-zero integer and everything else returns zero.
    for (int ch = 0; ch < 255; ++ch) {
        if (ch == ' ' || ch == '\t')
            EXPECT_NE(call_isblank(ch), 0);
        else
            EXPECT_EQ(call_isblank(ch), 0);
    }
}

//===-- Unittests for iscntrl --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsCntrl, DefaultLocale)
{
    // Loops through all characters, verifying that control characters
    // return a non-zero integer, all others return zero.
    for (int ch = 0; ch < 255; ++ch) {
        if ((0 <= ch && ch <= 0x1f /*US*/) || ch == 0x7f /*DEL*/)
            EXPECT_NE(call_iscntrl(ch), 0);
        else
            EXPECT_EQ(call_iscntrl(ch), 0);
    }
}

//===-- Unittests for isdigit --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsDigit, DefaultLocale)
{
    // Loops through all characters, verifying that numbers return a
    // non-zero integer and everything else returns zero.
    for (int ch = 0; ch < 255; ++ch) {
        if ('0' <= ch && ch <= '9')
            EXPECT_NE(call_isdigit(ch), 0);
        else
            EXPECT_EQ(call_isdigit(ch), 0);
    }
}

//===-- Unittests for isgraph --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsGraph, DefaultLocale)
{
    // Loops through all characters, verifying that graphical characters
    // return a non-zero integer, everything else returns zero.
    for (int ch = 0; ch < 255; ++ch) {
        if ('!' <= ch && ch <= '~') // A-Z, a-z, 0-9, punctuation.
            EXPECT_NE(call_isgraph(ch), 0);
        else
            EXPECT_EQ(call_isgraph(ch), 0);
    }
}

//===-- Unittests for islower --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsLower, DefaultLocale)
{
    // Loops through all characters, verifying that lowercase letters
    // return a non-zero integer and everything else returns zero.
    for (int ch = 0; ch < 255; ++ch) {
        if ('a' <= ch && ch <= 'z')
            EXPECT_NE(call_islower(ch), 0);
        else
            EXPECT_EQ(call_islower(ch), 0);
    }
}

//===-- Unittests for isprint --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsPrint, DefaultLocale)
{
    for (int ch = 0; ch < 255; ++ch) {
        if (' ' <= ch && ch <= '~') // A-Z, a-z, 0-9, punctuation, space.
            EXPECT_NE(call_isprint(ch), 0);
        else
            EXPECT_EQ(call_isprint(ch), 0);
    }
}

//===-- Unittests for ispunct --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Helper function to mark the sections of the ASCII table that are
// punctuation characters. These are listed below:
//  Decimal    |         Symbol
//  -----------------------------------------
//  33 -  47   |  ! " $ % & ' ( ) * + , - . /
//  58 -  64   |  : ; < = > ? @
//  91 -  96   |  [ \ ] ^ _ `
// 123 - 126   |  { | } ~
static inline int is_punctuation_character(int c)
{
    return ('!' <= c && c <= '/') || (':' <= c && c <= '@') ||
        ('[' <= c && c <= '`') || ('{' <= c && c <= '~');
}

TEST(LlvmLibcIsPunct, DefaultLocale)
{
    // Loops through all characters, verifying that punctuation characters
    // return a non-zero integer, and everything else returns zero.
    for (int ch = 0; ch < 255; ++ch) {
        if (is_punctuation_character(ch))
            EXPECT_NE(call_ispunct(ch), 0);
        else
            EXPECT_EQ(call_ispunct(ch), 0);
    }
}

//===-- Unittests for isspace --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsSpace, DefaultLocale)
{
    // Loops through all characters, verifying that space characters
    // return true and everything else returns false.
    // Hexadecimal | Symbol
    // ---------------------------
    //    0x09     |   horizontal tab
    //    0x0a     |   line feed
    //    0x0b     |   vertical tab
    //    0x0d     |   carriage return
    //    0x20     |   space
    for (int ch = 0; ch < 255; ++ch) {
        if (ch == 0x20 || (0x09 <= ch && ch <= 0x0d))
            EXPECT_NE(call_isspace(ch), 0);
        else
            EXPECT_EQ(call_isspace(ch), 0);
    }
}

//===-- Unittests for isupper --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsUpper, DefaultLocale)
{
    // Loops through all characters, verifying that uppercase letters
    // return a non-zero integer and everything else returns zero.
    for (int ch = 0; ch < 255; ++ch) {
        if ('A' <= ch && ch <= 'Z')
            EXPECT_NE(call_isupper(ch), 0);
        else
            EXPECT_EQ(call_isupper(ch), 0);
    }
}

//===-- Unittests for isxdigit -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcIsXDigit, DefaultLocale)
{
    for (int ch = 0; ch < 255; ++ch) {
        if (('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') ||
            ('A' <= ch && ch <= 'F'))
            EXPECT_NE(call_isxdigit(ch), 0);
        else
            EXPECT_EQ(call_isxdigit(ch), 0);
    }
}

//===-- Unittests for toascii --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcToAscii, DefaultLocale)
{
    // Loops through all characters, verifying that ascii characters
    //    (which are all 7 bit unsigned integers)
    // return themself, and that all other characters return themself
    // mod 128 (which is equivalent to & 0x7f)
    for (int ch = 0; ch < 255; ++ch) {
        if (ch <= 0x7f)
            EXPECT_EQ(call_toascii(ch), ch);
        else
            EXPECT_EQ(call_toascii(ch), ch & 0x7f);
    }
}

//===-- Unittests for tolower --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcToLower, DefaultLocale)
{
    for (int ch = 0; ch < 255; ++ch) {
        // This follows pattern 'A' + 32 = 'a'.
        if ('A' <= ch && ch <= 'Z')
            EXPECT_EQ(call_tolower(ch), ch + 32);
        else
            EXPECT_EQ(call_tolower(ch), ch);
    }
}

//===-- Unittests for toupper --------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcToUpper, DefaultLocale)
{
    for (int ch = 0; ch < 255; ++ch) {
        // This follows pattern 'a' - 32 = 'A'.
        if ('a' <= ch && ch <= 'z')
            EXPECT_EQ(call_toupper(ch), ch - 32);
        else
            EXPECT_EQ(call_toupper(ch), ch);
    }
}