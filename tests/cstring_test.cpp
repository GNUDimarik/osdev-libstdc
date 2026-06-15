//===-- Unittests for bcmp ------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include <gtest/gtest.h>
#include "../include/posix/posix_strings.h"
#include "cstring.h"
#include "utils.h"
#include <vector>

TEST(LlvmLibcBcmpTest, CmpZeroByte)
{
    const char *lhs = "ab";
    const char *rhs = "bc";
    EXPECT_EQ(__STDLIBC_TEST_NAMESPACE::bcmp(lhs, rhs, 0), 0);
}

TEST(LlvmLibcBcmpTest, LhsRhsAreTheSame)
{
    const char *lhs = "ab";
    const char *rhs = "ab";
    EXPECT_EQ(__STDLIBC_TEST_NAMESPACE::bcmp(lhs, rhs, 2), 0);
}

TEST(LlvmLibcBcmpTest, LhsBeforeRhsLexically)
{
    const char *lhs = "ab";
    const char *rhs = "ac";
    EXPECT_NE(__STDLIBC_TEST_NAMESPACE::bcmp(lhs, rhs, 2), 0);
}

TEST(LlvmLibcBcmpTest, LhsAfterRhsLexically)
{
    const char *lhs = "ac";
    const char *rhs = "ab";
    EXPECT_NE(__STDLIBC_TEST_NAMESPACE::bcmp(lhs, rhs, 2), 0);
}

TEST(LlvmLibcBcmpTest, Sweep)
{
    static constexpr size_t K_MAX_SIZE = 1024;
    char lhs[K_MAX_SIZE];
    char rhs[K_MAX_SIZE];

    const auto reset = [](char *const ptr)
    {
        for (size_t i = 0; i < K_MAX_SIZE; ++i)
            ptr[i] = 'a';
    };

    reset(lhs);
    reset(rhs);
    for (size_t i = 0; i < K_MAX_SIZE; ++i)
        EXPECT_EQ(__STDLIBC_TEST_NAMESPACE::bcmp(lhs, rhs, i), 0);

    reset(lhs);
    reset(rhs);
    for (size_t i = 0; i < K_MAX_SIZE; ++i) {
        rhs[i] = 'b';
        EXPECT_NE(__STDLIBC_TEST_NAMESPACE::bcmp(lhs, rhs, K_MAX_SIZE), 0);
        rhs[i] = 'a';
    }
}

//===-- Unittests for bzero -----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
using Data = std::vector<char>;

static constexpr const char *k_deadcode = "DEADC0DE";

// Returns a Data object filled with a repetition of `filler`.
Data bzero_get_data(const char *filler)
{
    Data out;
    out.resize(2048);

    for (size_t i = 0; i < out.size(); ++i)
        out[i] = filler[i % strlen(filler)];
    return std::move(out);
}

TEST(LlvmLibcBzeroTest, Thorough)
{
    const Data dirty = bzero_get_data(k_deadcode);
    for (size_t count = 0; count < 1024; ++count) {
        for (size_t align = 0; align < 64; ++align) {
            auto buffer = dirty;
            char *const dst = &buffer[align];
            __STDLIBC_TEST_NAMESPACE::bzero(dst, count);
            // Everything before copy is untouched.
            for (size_t i = 0; i < align; ++i)
                ASSERT_EQ(buffer[i], dirty[i]);
            // Everything in between is copied.
            for (size_t i = 0; i < count; ++i)
                ASSERT_EQ(buffer[align + i], char(0));
            // Everything after copy is untouched.
            for (size_t i = align + count; i < dirty.size(); ++i)
                ASSERT_EQ(buffer[i], dirty[i]);
        }
    }
}

// FIXME: Add tests with reads and writes on the boundary of a read/write
// protected page to check we're not reading nor writing prior/past the allowed
// regions.
/* This part is ported from glibc fss test */

void do_try(const char *name, long long int param, int value, int expected)
{
    EXPECT_EQ(value, expected);
#if 0
    if (value != expected)
    {
      printf ("%s(%#llx) expected %d got %d\n",
          name, param, expected, value);
      support_record_failure ();
    }
  else
    printf ("%s(%#llx) as expected %d\n", name, param, value);
#endif
}


#define DO_TEST(fct, type) {\
  int i; \
  do_try (#fct, 0, fct ((type) 0), 0);                          \
  for (i=0 ; i < 8 * sizeof (type); i++)                      \
    do_try (#fct, 1ll << i, fct (((type) 1) << i), i + 1);                  \
  for (i=0 ; i < 8 * sizeof (type) ; i++)                      \
    do_try (#fct, (~((type) 0) >> i) << i, fct ((~((type) 0) >> i) << i), i + 1);\
  do_try (#fct, 0x80008000, fct ((type) 0x80008000), 16); } \


TEST(LibcFFsIntTest, Equal)
{
    for (int i = 0; i < 1000; i++) {
        EXPECT_EQ(__STDLIBC_TEST_NAMESPACE::ffs(i), __builtin_ffs(i));
    }

    DO_TEST (__STDLIBC_TEST_NAMESPACE::ffs, int);
}

TEST(LibcFFsLongIntTest, Equal)
{
    for (long int i = 0; i < 1000; i++) {
        EXPECT_EQ(__STDLIBC_TEST_NAMESPACE::ffsl(i), __builtin_ffsl(i));
    }

    DO_TEST (__STDLIBC_TEST_NAMESPACE::ffsl, long int);
}

TEST(LibcFFsLongLongIntTest, Equal)
{
    for (long long int i = 0; i < 1000; i++) {
        EXPECT_EQ(__STDLIBC_TEST_NAMESPACE::ffsll(i), __builtin_ffsll(i));
    }

    DO_TEST (__STDLIBC_TEST_NAMESPACE::ffsll, long long int);
}

//===-- Unittests for memccpy ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
static void check_memccpy(std::vector<char> dst,
                          const std::vector<char> src, int end,
                          size_t count,
                          const std::vector<char> expected,
                          size_t expectedCopied, bool shouldReturnNull = false)
{
    // Making sure we don't overflow buffer.
    ASSERT_GE(dst.size(), count);
    // Making sure memccpy returns dst.
    void *result = __STD_NAMESPACE::memccpy(dst.data(), src.data(), end, count);

    if (shouldReturnNull) {
        ASSERT_EQ(result, static_cast<void *>(nullptr));
    }
    else {
        ASSERT_EQ(result, static_cast<void *>(dst.data() + expectedCopied));
    }

    // Expected must be of the same size as dst.
    ASSERT_EQ(dst.size(), expected.size());
    // Expected and dst are the same.
    for (size_t i = 0; i < expected.size(); ++i)
        ASSERT_EQ(expected[i], dst[i]);
}

TEST(LlvmLibcMemccpyTest, UntouchedUnrelatedEnd)
{
    std::vector<char> dst = {'a', 'b'};
    const std::vector<char> src = {'x', '\0'};
    const std::vector<char> expected = {'a', 'b'};
    check_memccpy(dst, src, 'z', 0, expected, 0, true);
}

TEST(LlvmLibcMemccpyTest, UntouchedStartsWithEnd)
{
    std::vector<char> dst = {'a', 'b'};
    const std::vector<char> src = {'x', '\0'};
    const std::vector<char> expected = {'a', 'b'};
    check_memccpy(dst, src, 'x', 0, expected, 0, true);
}

TEST(LlvmLibcMemccpyTest, CopyOneUnrelatedEnd)
{
    std::vector<char> dst = {'a', 'b'};
    const std::vector<char> src = {'x', 'y'};
    const std::vector<char> expected = {'x', 'b'};
    check_memccpy(dst, src, 'z', 1, expected, 1, true);
}

TEST(LlvmLibcMemccpyTest, CopyOneStartsWithEnd)
{
    std::vector<char> dst = {'a', 'b'};
    const std::vector<char> src = {'x', 'y'};
    const std::vector<char> expected = {'x', 'b'};
    check_memccpy(dst, src, 'x', 1, expected, 1);
}

TEST(LlvmLibcMemccpyTest, CopyTwoUnrelatedEnd)
{
    std::vector<char> dst = {'a', 'b'};
    const std::vector<char> src = {'x', 'y'};
    const std::vector<char> expected = {'x', 'y'};
    check_memccpy(dst, src, 'z', 2, expected, 2, true);
}

TEST(LlvmLibcMemccpyTest, CopyTwoStartsWithEnd)
{
    std::vector<char> dst = {'a', 'b'};
    const std::vector<char> src = {'x', 'y'};
    const std::vector<char> expected = {'x', 'b'};
    check_memccpy(dst, src, 'x', 2, expected, 1);
}

//===-- Unittests for memchr ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// A helper function that calls memchr and abstracts away the explicit cast for
// readability purposes.
const char *call_memchr(const void *src, int c, size_t size)
{
    return reinterpret_cast<const char *>(__STD_NAMESPACE::memchr(src, c, size));
}

TEST(LlvmLibcMemChrTest, FindsCharacterAfterNullTerminator)
{
    // memchr should continue searching after a null terminator.
    const size_t size = 5;
    const unsigned char src[size] = {'a', '\0', 'b', 'c', '\0'};
    // Should return 'b', 'c', '\0' even when after null terminator.
    ASSERT_STREQ(call_memchr(src, 'b', size), "bc");
}

TEST(LlvmLibcMemChrTest, FindsCharacterInNonNullTerminatedCollection)
{
    const size_t size = 3;
    const unsigned char src[size] = {'a', 'b', 'c'};
    // Should return 'b', 'c'.
    const char *ret = call_memchr(src, 'b', size);
    ASSERT_EQ(ret[0], 'b');
    ASSERT_EQ(ret[1], 'c');
}

TEST(LlvmLibcMemChrTest, FindsFirstCharacter)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    // Should return original array since 'a' is the first character.
    ASSERT_STREQ(call_memchr(src, 'a', size), "abcde");
}

TEST(LlvmLibcMemChrTest, FindsMiddleCharacter)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    // Should return characters after (and including) 'c'.
    ASSERT_STREQ(call_memchr(src, 'c', size), "cde");
}

TEST(LlvmLibcMemChrTest, FindsLastCharacterThatIsNotNullTerminator)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    // Should return 'e' and null-terminator.
    ASSERT_STREQ(call_memchr(src, 'e', size), "e");
}

TEST(LlvmLibcMemChrTest, FindsNullTerminator)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    // Should return null terminator.
    ASSERT_STREQ(call_memchr(src, '\0', size), "");
}

TEST(LlvmLibcMemChrTest, CharacterNotWithinStringShouldReturnNullptr)
{
    const size_t size = 4;
    const unsigned char src[size] = {'1', '2', '3', '?'};
    // Since 'z' is not within 'characters', should return nullptr.
    ASSERT_STREQ(call_memchr(src, 'z', size), nullptr);
}

TEST(LlvmLibcMemChrTest, CharacterNotWithinSizeShouldReturnNullptr)
{
    const unsigned char src[5] = {'1', '2', '3', '4', '\0'};
    // Since '4' is not the first or second character, this should return nullptr.
    const size_t size = 2;
    ASSERT_STREQ(call_memchr(src, '4', size), nullptr);
}

TEST(LlvmLibcMemChrTest, TheSourceShouldNotChange)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    const char *src_copy = reinterpret_cast<const char *>(src);
    // When the character is found, the source string should not change.
    __STD_NAMESPACE::memchr(src, 'd', size);
    ASSERT_STREQ(reinterpret_cast<const char *>(src), src_copy);
    // Same case for when the character is not found.
    __STD_NAMESPACE::memchr(src, 'z', size);
    ASSERT_STREQ(reinterpret_cast<const char *>(src), src_copy);
}

TEST(LlvmLibcMemChrTest, ShouldFindFirstOfDuplicates)
{
    const size_t size = 12; // 11 characters + null terminator.
    const char *dups = "abc1def1ghi";
    // 1 is duplicated in 'dups', but it should find the first copy.
    ASSERT_STREQ(call_memchr(dups, '1', size), "1def1ghi");
}

TEST(LlvmLibcMemChrTest, EmptyStringShouldOnlyMatchNullTerminator)
{
    const size_t size = 1; // Null terminator.
    const char *empty_string = "";
    // Null terminator should match.
    ASSERT_STREQ(call_memchr(empty_string, '\0', size), "");
    // All other characters should not match.
    ASSERT_STREQ(call_memchr(empty_string, 'A', size), nullptr);
    ASSERT_STREQ(call_memchr(empty_string, '9', size), nullptr);
    ASSERT_STREQ(call_memchr(empty_string, '?', size), nullptr);
}

TEST(LlvmLibcMemChrTest, SingleRepeatedCharacterShouldReturnFirst)
{
    const char *dups = "XXXXX";
    const size_t size = 6; // 5 characters + null terminator.
    // Should return original string since X is first character.
    ASSERT_STREQ(call_memchr(dups, 'X', size), dups);
}

TEST(LlvmLibcMemChrTest, SignedCharacterFound)
{
    char c = -1;
    const size_t size = 1;
    char src[size] = {c};
    const char *actual = call_memchr(src, c, size);
    // Should find the first character 'c'.
    ASSERT_EQ(actual[0], c);
}

//===-- Unittests for memcmp ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcMemcmpTest, CmpZeroByte)
{
    const char *lhs = "ab";
    const char *rhs = "yz";
    EXPECT_EQ(__STD_NAMESPACE::memcmp(lhs, rhs, 0), 0);
}

TEST(LlvmLibcMemcmpTest, LhsRhsAreTheSame)
{
    const char *lhs = "ab";
    const char *rhs = "ab";
    EXPECT_EQ(__STD_NAMESPACE::memcmp(lhs, rhs, 2), 0);
}

TEST(LlvmLibcMemcmpTest, LhsBeforeRhsLexically)
{
    const char *lhs = "ab";
    const char *rhs = "az";
    EXPECT_LT(__STD_NAMESPACE::memcmp(lhs, rhs, 2), 0);
}

TEST(LlvmLibcMemcmpTest, LhsAfterRhsLexically)
{
    const char *lhs = "az";
    const char *rhs = "ab";
    EXPECT_GT(__STD_NAMESPACE::memcmp(lhs, rhs, 2), 0);
}

TEST(LlvmLibcMemcmpTest, Sweep)
{
    static constexpr size_t K_MAX_SIZE = 1024;
    char lhs[K_MAX_SIZE];
    char rhs[K_MAX_SIZE];

    const auto reset = [](char *const ptr)
    {
        for (size_t i = 0; i < K_MAX_SIZE; ++i)
            ptr[i] = 'a';
    };

    reset(lhs);
    reset(rhs);
    for (size_t i = 0; i < K_MAX_SIZE; ++i)
        ASSERT_EQ(__STD_NAMESPACE::memcmp(lhs, rhs, i), 0);

    reset(lhs);
    reset(rhs);
    for (size_t i = 0; i < K_MAX_SIZE; ++i) {
        rhs[i] = 'z';
        ASSERT_LT(__STD_NAMESPACE::memcmp(lhs, rhs, K_MAX_SIZE), 0);
        rhs[i] = 'a';
    }
}

//===-- Unittests for memcpy ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

using Data = std::vector<char>;

static constexpr const char *k_numbers = "0123456789";

// Returns a Data object filled with a repetition of `filler`.
Data memcpy_get_data(const char *filler)
{
    Data out;
    out.resize(2048);
    for (size_t i = 0; i < out.size(); ++i)
        out[i] = filler[i % strlen(filler)];
    return out;
}

TEST(LlvmLibcMemcpyTest, Thorough)
{
    const Data groundtruth = memcpy_get_data(k_numbers);
    const Data dirty = memcpy_get_data(k_deadcode);
    for (size_t count = 0; count < 1024; ++count) {
        for (size_t align = 0; align < 64; ++align) {
            auto buffer = dirty;
            const char *const src = groundtruth.data();
            void *const dst = &buffer[align];
            void *const ret = __STD_NAMESPACE::memcpy(dst, src, count);
            // Return value is `dst`.
            ASSERT_EQ(ret, dst);
            // Everything before copy is untouched.
            for (size_t i = 0; i < align; ++i)
                ASSERT_EQ(buffer[i], dirty[i]);
            // Everything in between is copied.
            for (size_t i = 0; i < count; ++i)
                ASSERT_EQ(buffer[align + i], groundtruth[i]);
            // Everything after copy is untouched.
            for (size_t i = align + count; i < dirty.size(); ++i)
                ASSERT_EQ(buffer[i], dirty[i]);
        }
    }
}

// FIXME: Add tests with reads and writes on the boundary of a read/write
// protected page to check we're not reading nor writing prior/past the allowed
// regions.

//===-- Unittests for memmove ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcMemmoveTest, MoveZeroByte)
{
    char Buffer[] = {'a', 'b', 'y', 'z'};
    const char Expected[] = {'a', 'b', 'y', 'z'};
    void *const Dst = Buffer;
    void *const Ret = __STD_NAMESPACE::memmove(Dst, Buffer + 2, 0);
    EXPECT_EQ(Ret, Dst);
    EXPECT_EQ(__STD_NAMESPACE::memcmp(Buffer, Expected, ARRAY_SIZE(Buffer)), 0);
}

TEST(LlvmLibcMemmoveTest, DstAndSrcPointToSameAddress)
{
    char Buffer[] = {'a', 'b'};
    const char Expected[] = {'a', 'b'};
    void *const Dst = Buffer;
    void *const Ret = __STD_NAMESPACE::memmove(Dst, Buffer, 1);
    EXPECT_EQ(Ret, Dst);
    EXPECT_EQ(__STD_NAMESPACE::memcmp(Buffer, Expected, ARRAY_SIZE(Buffer)), 0);
}

TEST(LlvmLibcMemmoveTest, DstStartsBeforeSrc)
{
    // Set boundary at beginning and end for not overstepping when
    // copy forward or backward.
    char Buffer[] = {'z', 'a', 'b', 'c', 'z'};
    const char Expected[] = {'z', 'b', 'c', 'c', 'z'};
    void *const Dst = Buffer + 1;
    void *const Ret = __STD_NAMESPACE::memmove(Dst, Buffer + 2, 2);
    EXPECT_EQ(Ret, Dst);
    EXPECT_EQ(__STD_NAMESPACE::memcmp(Buffer, Expected, ARRAY_SIZE(Buffer)), 0);
}

TEST(LlvmLibcMemmoveTest, DstStartsAfterSrc)
{
    char Buffer[] = {'z', 'a', 'b', 'c', 'z'};
    const char Expected[] = {'z', 'a', 'a', 'b', 'z'};
    void *const Dst = Buffer + 2;
    void *const Ret = __STD_NAMESPACE::memmove(Dst, Buffer + 1, 2);
    EXPECT_EQ(Ret, Dst);
    EXPECT_EQ(__STD_NAMESPACE::memcmp(Buffer, Expected, ARRAY_SIZE(Buffer)), 0);
}

// e.g. `Dst` follow `src`.
// str: [abcdefghij]
//      [__src_____]
//      [_____Dst__]
TEST(LlvmLibcMemmoveTest, SrcFollowDst)
{
    char Buffer[] = {'z', 'a', 'b', 'z'};
    const char Expected[] = {'z', 'b', 'b', 'z'};
    void *const Dst = Buffer + 1;
    void *const Ret = __STD_NAMESPACE::memmove(Dst, Buffer + 2, 1);
    EXPECT_EQ(Ret, Dst);
    EXPECT_EQ(__STD_NAMESPACE::memcmp(Buffer, Expected, ARRAY_SIZE(Buffer)), 0);
}

TEST(LlvmLibcMemmoveTest, DstFollowSrc)
{
    char Buffer[] = {'z', 'a', 'b', 'z'};
    const char Expected[] = {'z', 'a', 'a', 'z'};
    void *const Dst = Buffer + 2;
    void *const Ret = __STD_NAMESPACE::memmove(Dst, Buffer + 1, 1);
    EXPECT_EQ(Ret, Dst);
    EXPECT_EQ(__STD_NAMESPACE::memcmp(Buffer, Expected, ARRAY_SIZE(Buffer)), 0);
}

static constexpr int kMaxSize = 512;

char GetRandomChar()
{
    static constexpr const uint64_t A = 1103515245;
    static constexpr const uint64_t C = 12345;
    static constexpr const uint64_t M = 1ULL << 31;
    static uint64_t Seed = 123456789;
    Seed = (A * Seed + C) % M;
    return Seed;
}

void Randomize(std::vector<char> Buffer)
{
    for (auto &current: Buffer)
        current = GetRandomChar();
}

TEST(LlvmLibcMemmoveTest, Thorough)
{
    using LargeBuffer = std::vector<char>;
    LargeBuffer GroundTruth;
    GroundTruth.resize(3 * kMaxSize);
    Randomize(GroundTruth);
    for (int Size = 0; Size < kMaxSize; ++Size) {
        for (int Offset = -Size; Offset < Size; ++Offset) {
            LargeBuffer Buffer = GroundTruth;
            LargeBuffer Expected = GroundTruth;
            size_t DstOffset = kMaxSize;
            size_t SrcOffset = kMaxSize + Offset;
            for (int I = 0; I < Size; ++I)
                Expected[DstOffset + I] = GroundTruth[SrcOffset + I];
            void *const Dst = Buffer.data() + DstOffset;
            void *const Ret =
                __STD_NAMESPACE::memmove(Dst, Buffer.data() + SrcOffset, Size);
            EXPECT_EQ(Ret, Dst);
            EXPECT_EQ(__STD_NAMESPACE::memcmp(Buffer.data(), Expected.data(), Buffer.size()), 0);
        }
    }
}

//===-- Unittests for mempcpy ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Since this function just calls out to memcpy, and memcpy has its own unit
// tests, it is assumed that memcpy works. These tests are just for the specific
// mempcpy behavior (returning the end of what was copied).
TEST(LlvmLibcMempcpyTest, Simple)
{
    const char *src = "12345";
    char dest[10];
    void *result = __STD_NAMESPACE::mempcpy(dest, src, 6);
    ASSERT_EQ(static_cast<char *>(result), dest + 6);
    ASSERT_STREQ(src, dest);
}

TEST(LlvmLibcMempcpyTest, ZeroCount)
{
    const char *src = "12345";
    char dest[10];
    void *result = __STD_NAMESPACE::mempcpy(dest, src, 0);
    ASSERT_EQ(static_cast<char *>(result), dest);
}

//===-- Unittests for memrchr ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// A helper function that calls memrchr and abstracts away the explicit cast for
// readability purposes.
const char *call_memrchr(const void *src, int c, size_t size)
{
    return reinterpret_cast<const char *>(__STD_NAMESPACE::memrchr(src, c, size));
}

TEST(LlvmLibcMemRChrTest, FindsCharacterAfterNullTerminator)
{
    // memrchr should continue searching after a null terminator.
    const size_t size = 6;
    const unsigned char src[size] = {'a', '\0', 'b', 'c', 'd', '\0'};
    // Should return 'b', 'c', 'd', '\0' even when after null terminator.
    ASSERT_STREQ(call_memrchr(src, 'b', size), "bcd");
}

TEST(LlvmLibcMemRChrTest, FindsCharacterInNonNullTerminatedCollection)
{
    const size_t size = 3;
    const unsigned char src[size] = {'a', 'b', 'c'};
    // Should return 'b', 'c'.
    const char *ret = call_memrchr(src, 'b', size);
    ASSERT_EQ(ret[0], 'b');
    ASSERT_EQ(ret[1], 'c');
}

TEST(LlvmLibcMemRChrTest, FindsFirstCharacter)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    // Should return original array since 'a' is the first character.
    ASSERT_STREQ(call_memrchr(src, 'a', size), "abcde");
}

TEST(LlvmLibcMemRChrTest, FindsMiddleCharacter)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    // Should return characters after (and including) 'c'.
    ASSERT_STREQ(call_memrchr(src, 'c', size), "cde");
}

TEST(LlvmLibcMemRChrTest, FindsLastCharacterThatIsNotNullTerminator)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    // Should return 'e' and null-terminator.
    ASSERT_STREQ(call_memrchr(src, 'e', size), "e");
}

TEST(LlvmLibcMemRChrTest, FindsNullTerminator)
{
    const size_t size = 6;
    const unsigned char src[size] = {'a', 'b', 'c', 'd', 'e', '\0'};
    // Should return null terminator.
    ASSERT_STREQ(call_memrchr(src, '\0', size), "");
}

TEST(LlvmLibcMemRChrTest, CharacterNotWithinStringShouldReturnNullptr)
{
    const size_t size = 4;
    const unsigned char src[size] = {'1', '2', '3', '?'};
    // Since 'z' is not within 'characters', should return nullptr.
    ASSERT_STREQ(call_memrchr(src, 'z', size), nullptr);
}

TEST(LlvmLibcMemRChrTest, CharacterNotWithinSizeShouldReturnNullptr)
{
    const unsigned char src[5] = {'1', '2', '3', '4', '\0'};
    // Since '4' is not within the first 2 characters, this should return nullptr.
    const size_t size = 2;
    ASSERT_STREQ(call_memrchr(src, '4', size), nullptr);
}

TEST(LlvmLibcMemRChrTest, ShouldFindLastOfDuplicates)
{
    size_t size = 12; // 11 characters + null terminator.
    const char *dups = "abc1def1ghi";
    // 1 is duplicated in 'dups', but it should find the last copy.
    ASSERT_STREQ(call_memrchr(dups, '1', size), "1ghi");

    const char *repeated = "XXXXX";
    size = 6; // 5 characters + null terminator.
    // Should return the last X with the null terminator.
    ASSERT_STREQ(call_memrchr(repeated, 'X', size), "X");
}

TEST(LlvmLibcMemRChrTest, EmptyStringShouldOnlyMatchNullTerminator)
{
    const size_t size = 1; // Null terminator.
    const char *empty_string = "";
    // Null terminator should match.
    ASSERT_STREQ(call_memrchr(empty_string, '\0', size), "");
    // All other characters should not match.
    ASSERT_STREQ(call_memrchr(empty_string, 'A', size), nullptr);
    ASSERT_STREQ(call_memrchr(empty_string, '9', size), nullptr);
    ASSERT_STREQ(call_memrchr(empty_string, '?', size), nullptr);
}

TEST(LlvmLibcMemRChrTest, SignedCharacterFound)
{
    char c = -1;
    const size_t size = 1;
    char src[size] = {c};
    const char *actual = call_memrchr(src, c, size);
    // Should find the last character 'c'.
    ASSERT_EQ(actual[0], c);
}

TEST(LlvmLibcMemRChrTest, ZeroLengthShouldReturnNullptr)
{
    const unsigned char src[4] = {'a', 'b', 'c', '\0'};
    // This will iterate over exactly zero characters, so should return nullptr.
    ASSERT_STREQ(call_memrchr(src, 'd', 0), nullptr);
}

//===-- Unittests for memset ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

using Data = std::vector<char>;

// Returns a Data object filled with a repetition of `filler`.
Data get_data(const char *filler)
{
    Data out;
    out.resize(2048);

    for (size_t i = 0; i < out.size(); ++i)
        out[i] = filler[i % strlen(filler)];
    return out;
}

TEST(LlvmLibcMemsetTest, Thorough)
{
    const Data dirty = get_data(k_deadcode);
    for (int value = -1; value <= 1; ++value) {
        for (size_t count = 0; count < 1024; ++count) {
            for (size_t align = 0; align < 64; ++align) {
                auto buffer = dirty;
                void *const dst = &buffer[align];
                void *const ret = __STD_NAMESPACE::memset(dst, value, count);
                // Return value is `dst`.
                ASSERT_EQ(ret, dst);
                // Everything before copy is untouched.
                for (size_t i = 0; i < align; ++i)
                    ASSERT_EQ(buffer[i], dirty[i]);
                // Everything in between is copied.
                for (size_t i = 0; i < count; ++i)
                    ASSERT_EQ(buffer[align + i], (char) value);
                // Everything after copy is untouched.
                for (size_t i = align + count; i < dirty.size(); ++i)
                    ASSERT_EQ(buffer[i], dirty[i]);
            }
        }
    }
}

// FIXME: Add tests with reads and writes on the boundary of a read/write
// protected page to check we're not reading nor writing prior/past the allowed
// regions.

//===-- Unittests for stpcpy ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStpCpyTest, EmptySrc)
{
    const char *empty = "";
    size_t src_size = __STD_NAMESPACE::strlen(empty);
    char dest[4] = {'a', 'b', 'c', '\0'};

    char *result = __STD_NAMESPACE::stpcpy(dest, empty);
    ASSERT_EQ(dest + src_size, result);
    ASSERT_EQ(result[0], '\0');
    ASSERT_STREQ(dest, empty);
}

TEST(LlvmLibcStpCpyTest, EmptyDest)
{
    const char *abc = "abc";
    size_t src_size = __STD_NAMESPACE::strlen(abc);
    char dest[4];

    char *result = __STD_NAMESPACE::stpcpy(dest, abc);
    ASSERT_EQ(dest + src_size, result);
    ASSERT_EQ(result[0], '\0');
    ASSERT_STREQ(dest, abc);
}

TEST(LlvmLibcStpCpyTest, OffsetDest)
{
    const char *abc = "abc";
    size_t src_size = __STD_NAMESPACE::strlen(abc);
    char dest[7] = {'x', 'y', 'z'};

    char *result = __STD_NAMESPACE::stpcpy(dest + 3, abc);
    ASSERT_EQ(dest + 3 + src_size, result);
    ASSERT_EQ(result[0], '\0');
    ASSERT_STREQ(dest, "xyzabc");
}

//===-- Unittests for strcasecmp ----------------------------------------------===//
// This is ported strcmp test from LLVM
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrCaseCmpTest, EmptyStringsShouldReturnZero)
{
    const char *s1 = "";
    const char *s2 = "";
    int result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s1, s2);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s2, s1);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrCaseCmpTest, EmptyStringShouldNotEqualNonEmptyString)
{
    const char *empty = "";
    const char *s2 = "abc";
    int result = __STDLIBC_TEST_NAMESPACE::strcasecmp(empty, s2);
    // This should be '\0' - 'a' = -97
    ASSERT_EQ(result, -97);

    // Similar case if empty string is second argument.
    const char *s3 = "123";
    result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s3, empty);
    // This should be '1' - '\0' = 49
    ASSERT_EQ(result, 49);
}

TEST(LlvmLibcStrCaseCmpTest, EqualStringsShouldReturnZero)
{
    const char *s1 = "aBc";
    const char *s2 = "abc";
    int result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s1, s2);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s2, s1);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrCaseCmpTest, ShouldReturnResultOfFirstDifference)
{
    const char *s1 = "___B42__";
    const char *s2 = "___C55__";
    int result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s1, s2);
    // This should return 'B' - 'C' = -1.
    ASSERT_EQ(result, -1);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s2, s1);
    // This should return 'C' - 'B' = 1.
    ASSERT_EQ(result, 1);
}

TEST(LlvmLibcStrCaseCmpTest, CapitalizedLetterShouldNotBeEqual)
{
    const char *s1 = "ABCD";
    const char *s2 = "abCd";
    int result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s1, s2);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s2, s1);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrCaseCmpTest, UnequalLengthStringsShouldNotReturnZero)
{
    const char *s1 = "ABC";
    const char *s2 = "abcd";
    int result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s1, s2);
    // '\0' - 'd' = -100.
    ASSERT_EQ(result, -100);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strcasecmp(s2, s1);
    // 'd' - '\0' = 100.
    ASSERT_EQ(result, 100);
}

TEST(LlvmLibcStrCaseCmpTest, StringArgumentSwapChangesSign)
{
    const char *a = "a";
    const char *b = "B";
    int result = __STDLIBC_TEST_NAMESPACE::strcasecmp(b, a);
    // 'b' - 'a' = 1.
    ASSERT_EQ(result, 1);

    result = __STDLIBC_TEST_NAMESPACE::strcasecmp(a, b);
    // 'a' - 'b' = -1.
    ASSERT_EQ(result, -1);
}

//===-- Unittests for strcat ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrCatTest, EmptyDest)
{
    const char *abc = "abc";
    char dest[4];

    dest[0] = '\0';

    char *result = __STD_NAMESPACE::strcat(dest, abc);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, result);
    ASSERT_STREQ(dest, abc);
}

TEST(LlvmLibcStrCatTest, NonEmptyDest)
{
    const char *abc = "abc";
    char dest[7];

    dest[0] = 'x';
    dest[1] = 'y';
    dest[2] = 'z';
    dest[3] = '\0';

    char *result = __STD_NAMESPACE::strcat(dest, abc);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, result);
    ASSERT_STREQ(dest, "xyzabc");
}

//===-- Unittests for strchr ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrChrTest, FindsFirstCharacter)
{
    const char *src = "abcde";

    // Should return original string since 'a' is the first character.
    ASSERT_STREQ(__STD_NAMESPACE::strchr(src, 'a'), "abcde");
    // Source string should not change.
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrChrTest, FindsMiddleCharacter)
{
    const char *src = "abcde";

    // Should return characters after (and including) 'c'.
    ASSERT_STREQ(__STD_NAMESPACE::strchr(src, 'c'), "cde");
    // Source string should not change.
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrChrTest, FindsLastCharacterThatIsNotNullTerminator)
{
    const char *src = "abcde";

    // Should return 'e' and null-terminator.
    ASSERT_STREQ(__STD_NAMESPACE::strchr(src, 'e'), "e");
    // Source string should not change.
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrChrTest, FindsNullTerminator)
{
    const char *src = "abcde";

    // Should return null terminator.
    ASSERT_STREQ(__STD_NAMESPACE::strchr(src, '\0'), "");
    // Source string should not change.
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrChrTest, CharacterNotWithinStringShouldReturnNullptr)
{
    // Since 'z' is not within the string, should return nullptr.
    ASSERT_STREQ(__STD_NAMESPACE::strchr("123?", 'z'), nullptr);
}

TEST(LlvmLibcStrChrTest, TheSourceShouldNotChange)
{
    const char *src = "abcde";
    // When the character is found, the source string should not change.
    __STD_NAMESPACE::strchr(src, 'd');
    ASSERT_STREQ(src, "abcde");
    // Same case for when the character is not found.
    __STD_NAMESPACE::strchr(src, 'z');
    ASSERT_STREQ(src, "abcde");
    // Same case for when looking for nullptr.
    __STD_NAMESPACE::strchr(src, '\0');
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrChrTest, ShouldFindFirstOfDuplicates)
{
    // '1' is duplicated in the string, but it should find the first copy.
    ASSERT_STREQ(__STD_NAMESPACE::strchr("abc1def1ghi", '1'), "1def1ghi");

    const char *dups = "XXXXX";
    // Should return original string since 'X' is the first character.
    ASSERT_STREQ(__STD_NAMESPACE::strchr(dups, 'X'), dups);
}

TEST(LlvmLibcStrChrTest, EmptyStringShouldOnlyMatchNullTerminator)
{
    // Null terminator should match.
    ASSERT_STREQ(__STD_NAMESPACE::strchr("", '\0'), "");
    // All other characters should not match.
    ASSERT_STREQ(__STD_NAMESPACE::strchr("", 'Z'), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strchr("", '3'), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strchr("", '*'), nullptr);
}

//===-- Unittests for strcmp ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrCmpTest, EmptyStringsShouldReturnZero)
{
    const char *s1 = "";
    const char *s2 = "";
    int result = __STD_NAMESPACE::strcmp(s1, s2);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strcmp(s2, s1);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrCmpTest, EmptyStringShouldNotEqualNonEmptyString)
{
    const char *empty = "";
    const char *s2 = "abc";
    int result = __STD_NAMESPACE::strcmp(empty, s2);
    // This should be '\0' - 'a' = -97
    ASSERT_EQ(result, -97);

    // Similar case if empty string is second argument.
    const char *s3 = "123";
    result = __STD_NAMESPACE::strcmp(s3, empty);
    // This should be '1' - '\0' = 49
    ASSERT_EQ(result, 49);
}

TEST(LlvmLibcStrCmpTest, EqualStringsShouldReturnZero)
{
    const char *s1 = "abc";
    const char *s2 = "abc";
    int result = __STD_NAMESPACE::strcmp(s1, s2);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strcmp(s2, s1);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrCmpTest, ShouldReturnResultOfFirstDifference)
{
    const char *s1 = "___B42__";
    const char *s2 = "___C55__";
    int result = __STD_NAMESPACE::strcmp(s1, s2);
    // This should return 'B' - 'C' = -1.
    ASSERT_EQ(result, -1);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strcmp(s2, s1);
    // This should return 'C' - 'B' = 1.
    ASSERT_EQ(result, 1);
}

TEST(LlvmLibcStrCmpTest, CapitalizedLetterShouldNotBeEqual)
{
    const char *s1 = "abcd";
    const char *s2 = "abCd";
    int result = __STD_NAMESPACE::strcmp(s1, s2);
    // 'c' - 'C' = 32.
    ASSERT_EQ(result, 32);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strcmp(s2, s1);
    // 'C' - 'c' = -32.
    ASSERT_EQ(result, -32);
}

TEST(LlvmLibcStrCmpTest, UnequalLengthStringsShouldNotReturnZero)
{
    const char *s1 = "abc";
    const char *s2 = "abcd";
    int result = __STD_NAMESPACE::strcmp(s1, s2);
    // '\0' - 'd' = -100.
    ASSERT_EQ(result, -100);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strcmp(s2, s1);
    // 'd' - '\0' = 100.
    ASSERT_EQ(result, 100);
}

TEST(LlvmLibcStrCmpTest, StringArgumentSwapChangesSign)
{
    const char *a = "a";
    const char *b = "b";
    int result = __STD_NAMESPACE::strcmp(b, a);
    // 'b' - 'a' = 1.
    ASSERT_EQ(result, 1);

    result = __STD_NAMESPACE::strcmp(a, b);
    // 'a' - 'b' = -1.
    ASSERT_EQ(result, -1);
}

//===-- Unittests for strcpy ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrCpyTest, EmptySrc)
{
    const char *empty = "";
    char dest[4] = {'a', 'b', 'c', '\0'};

    char *result = __STD_NAMESPACE::strcpy(dest, empty);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, result);
    ASSERT_STREQ(dest, empty);
}

TEST(LlvmLibcStrCpyTest, EmptyDest)
{
    const char *abc = "abc";
    char dest[4];

    char *result = __STD_NAMESPACE::strcpy(dest, abc);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, result);
    ASSERT_STREQ(dest, abc);
}

TEST(LlvmLibcStrCpyTest, OffsetDest)
{
    const char *abc = "abc";
    char dest[7];

    dest[0] = 'x';
    dest[1] = 'y';
    dest[2] = 'z';

    char *result = __STD_NAMESPACE::strcpy(dest + 3, abc);
    ASSERT_EQ(dest + 3, result);
    ASSERT_STREQ(dest + 3, result);
    ASSERT_STREQ(dest, "xyzabc");
}

//===-- Unittests for strcspn ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrCSpnTest, ComplementarySpanShouldNotGoPastNullTerminator)
{
    const char src[5] = {'a', 'b', '\0', 'c', 'd'};
    EXPECT_EQ(__STD_NAMESPACE::strcspn(src, "b"), size_t{1});
    EXPECT_EQ(__STD_NAMESPACE::strcspn(src, "d"), size_t{2});

    // Same goes for the segment to be searched for.
    const char segment[5] = {'1', '2', '\0', '3', '4'};
    EXPECT_EQ(__STD_NAMESPACE::strcspn("123", segment), size_t{0});
}

TEST(LlvmLibcStrCSpnTest, ComplementarySpanForEachIndividualCharacter)
{
    const char *src = "12345";
    // The complementary span size should increment accordingly.
    EXPECT_EQ(__STD_NAMESPACE::strcspn(src, "1"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strcspn(src, "2"), size_t{1});
    EXPECT_EQ(__STD_NAMESPACE::strcspn(src, "3"), size_t{2});
    EXPECT_EQ(__STD_NAMESPACE::strcspn(src, "4"), size_t{3});
    EXPECT_EQ(__STD_NAMESPACE::strcspn(src, "5"), size_t{4});
}

TEST(LlvmLibcStrCSpnTest, ComplementarySpanIsStringLengthIfNoCharacterFound)
{
    // Null terminator.
    EXPECT_EQ(__STD_NAMESPACE::strcspn("", ""), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strcspn("", "_"), size_t{0});
    // Single character.
    EXPECT_EQ(__STD_NAMESPACE::strcspn("a", "b"), size_t{1});
    // Multiple characters.
    EXPECT_EQ(__STD_NAMESPACE::strcspn("abc", "1"), size_t{3});
}

TEST(LlvmLibcStrCSpnTest, DuplicatedCharactersNotPartOfComplementarySpan)
{
    // Complementary span should be zero in all these cases.
    EXPECT_EQ(__STD_NAMESPACE::strcspn("a", "aa"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strcspn("aa", "a"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strcspn("aaa", "aa"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strcspn("aaaa", "aa"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strcspn("aaaa", "baa"), size_t{0});
}

//===-- Unittests for strlen ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrLenTest, EmptyString)
{
    const char *empty = "";

    size_t result = __STD_NAMESPACE::strlen(empty);
    ASSERT_EQ((size_t) 0, result);
}

TEST(LlvmLibcStrLenTest, AnyString)
{
    const char *any = "Hello World!";

    size_t result = __STD_NAMESPACE::strlen(any);
    ASSERT_EQ((size_t) 12, result);
}

// Non standard extensions

TEST(LibcStrLvrTest, EmptyString)
{
    char empty[] = {""};

    char *result = __STD_NAMESPACE::strupr(empty);
    ASSERT_NE(result, static_cast<char *>(nullptr));
    ASSERT_EQ(empty, const_cast<const char *>(result));
    ASSERT_STREQ(empty, result);
}

TEST(LibcStrLvrTest, TextString)
{
    char array[] = {"Hello World!"};
    char out_array[] = {"HELLO WORLD!"};

    char *result = __STD_NAMESPACE::strupr(array);
    ASSERT_NE(result, static_cast<char *>(nullptr));
    ASSERT_EQ(array, const_cast<const char *>(result));
    ASSERT_STREQ(result, out_array);
}

TEST(LibcStrrevTest, EmptyString)
{
    char empty[] = {""};

    char *result = __STD_NAMESPACE::strrev(empty);
    ASSERT_NE(result, static_cast<char *>(nullptr));
    ASSERT_EQ(empty, const_cast<const char *>(result));
    ASSERT_STREQ(empty, result);
}

TEST(LibcStrrevTest, TextString)
{
    char array[] = {"Hello World!"};
    char out_array[] = {"!dlroW olleH"};

    char *result = __STD_NAMESPACE::strrev(array);
    ASSERT_NE(result, static_cast<char *>(nullptr));
    ASSERT_EQ(array, const_cast<const char *>(result));
    ASSERT_STREQ(result, out_array);
}

TEST(LibcStrUpprTest, EmptyString)
{
    char empty[] = {""};

    char *result = __STD_NAMESPACE::strupr(empty);
    ASSERT_NE(result, static_cast<char *>(nullptr));
    ASSERT_EQ(empty, const_cast<const char *>(result));
    ASSERT_STREQ(empty, result);
}

TEST(LibcStrUpprTest, TextString)
{
    char array[] = {"Hello World!"};
    char out_array[] = {"HELLO WORLD!"};

    char *result = __STD_NAMESPACE::strupr(array);
    ASSERT_NE(result, static_cast<char *>(nullptr));
    ASSERT_EQ(array, const_cast<const char *>(result));
    ASSERT_STREQ(result, out_array);
}

//===-- Unittests for strncasecmp ----------------------------------------------===//
// This is ported strncasecmp test from LLVM
//===----------------------------------------------------------------------===//

// This group is just copies of the strcmp tests, since all the same cases still
// need to be tested.

TEST(LlvmLibcStrnCaseCmpTest, EmptyStringsShouldReturnZeroWithSufficientLength) {
    const char *s1 = "";
    const char *s2 = "";
    int result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s1, s2, 1);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s2, s1, 1);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrnCaseCmpTest,
     EmptyStringShouldNotEqualNonEmptyStringWithSufficientLength) {
    const char *empty = "";
    const char *s2 = "abc";
    int result = __STDLIBC_TEST_NAMESPACE::strncasecmp(empty, s2, 3);
    // This should be '\0' - 'a' = -97
    ASSERT_EQ(result, -97);

    // Similar case if empty string is second argument.
    const char *s3 = "123";
    result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s3, empty, 3);
    // This should be '1' - '\0' = 49
    ASSERT_EQ(result, 49);
}

TEST(LlvmLibcStrnCaseCmpTest, EqualStringsShouldReturnZeroWithSufficientLength) {
    const char *s1 = "aBc";
    const char *s2 = "AbC";
    int result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s1, s2, 3);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s2, s1, 3);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrnCaseCmpTest,
     ShouldReturnResultOfFirstDifferenceWithSufficientLength) {
    const char *s1 = "___B42__";
    const char *s2 = "___C55__";
    int result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s1, s2, 8);
    // This should return 'B' - 'C' = -1.
    ASSERT_EQ(result, -1);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s2, s1, 8);
    // This should return 'C' - 'B' = 1.
    ASSERT_EQ(result, 1);
}

TEST(LlvmLibcStrnCaseCmpTest,
     CapitalizedLetterShouldNotBeEqualWithSufficientLength) {
    const char *s1 = "AbCd";
    const char *s2 = "aBcD";
    int result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s1, s2, 4);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s2, s1, 4);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrnCaseCmpTest,
     UnequalLengthStringsShouldNotReturnZeroWithSufficientLength) {
    const char *s1 = "aBc";
    const char *s2 = "AbcD";
    int result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s1, s2, 4);
    // '\0' - 'd' = -100.
    ASSERT_EQ(result, -100);

    // Verify operands reversed.
    result = __STDLIBC_TEST_NAMESPACE::strncasecmp(s2, s1, 4);
    // 'd' - '\0' = 100.
    ASSERT_EQ(result, 100);
}

//===-- Unittests for strncat ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrNCatTest, EmptyDest) {
    const char *abc = "abc";
    char dest[4];

    dest[0] = '\0';

    // Start by copying nothing
    char *result = __STD_NAMESPACE::strncat(dest, abc, 0);
    ASSERT_EQ(dest, result);
    ASSERT_EQ(dest[0], '\0');

    // Then copy part of it.
    result = __STD_NAMESPACE::strncat(dest, abc, 1);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, "a");

    // Reset for the last test.
    dest[0] = '\0';

    // Then copy all of it.
    result = __STD_NAMESPACE::strncat(dest, abc, 3);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, result);
    ASSERT_STREQ(dest, abc);
}

TEST(LlvmLibcStrNCatTest, NonEmptyDest) {
    const char *abc = "abc";
    char dest[7];

    dest[0] = 'x';
    dest[1] = 'y';
    dest[2] = 'z';
    dest[3] = '\0';

    // Copy only part of the string onto the end
    char *result = __STD_NAMESPACE::strncat(dest, abc, 1);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, "xyza");

    // Copy a bit more, but without resetting.
    result = __STD_NAMESPACE::strncat(dest, abc, 2);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, "xyzaab");

    // Set just the end marker, to make sure it overwrites properly.
    dest[3] = '\0';

    result = __STD_NAMESPACE::strncat(dest, abc, 3);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, "xyzabc");

    // Check that copying still works when count > src length
    dest[0] = '\0';
    // And that it doesn't write beyond what is necessary.
    dest[4] = 'Z';
    result = __STD_NAMESPACE::strncat(dest, abc, 4);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, "abc");
    ASSERT_EQ(dest[4], 'Z');

    result = __STD_NAMESPACE::strncat(dest, abc, 5);
    ASSERT_EQ(dest, result);
    ASSERT_STREQ(dest, "abcabc");
}

//===-- Unittests for strncmp ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// This group is just copies of the strcmp tests, since all the same cases still
// need to be tested.

TEST(LlvmLibcStrNCmpTest, EmptyStringsShouldReturnZeroWithSufficientLength) {
    const char *s1 = "";
    const char *s2 = "";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 1);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 1);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrNCmpTest,
     EmptyStringShouldNotEqualNonEmptyStringWithSufficientLength) {
    const char *empty = "";
    const char *s2 = "abc";
    int result = __STD_NAMESPACE::strncmp(empty, s2, 3);
    // This should be '\0' - 'a' = -97
    ASSERT_EQ(result, -97);

    // Similar case if empty string is second argument.
    const char *s3 = "123";
    result = __STD_NAMESPACE::strncmp(s3, empty, 3);
    // This should be '1' - '\0' = 49
    ASSERT_EQ(result, 49);
}

TEST(LlvmLibcStrNCmpTest, EqualStringsShouldReturnZeroWithSufficientLength) {
    const char *s1 = "abc";
    const char *s2 = "abc";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 3);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 3);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrNCmpTest,
     ShouldReturnResultOfFirstDifferenceWithSufficientLength) {
    const char *s1 = "___B42__";
    const char *s2 = "___C55__";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 8);
    // This should return 'B' - 'C' = -1.
    ASSERT_EQ(result, -1);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 8);
    // This should return 'C' - 'B' = 1.
    ASSERT_EQ(result, 1);
}

TEST(LlvmLibcStrNCmpTest,
     CapitalizedLetterShouldNotBeEqualWithSufficientLength) {
    const char *s1 = "abcd";
    const char *s2 = "abCd";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 4);
    // 'c' - 'C' = 32.
    ASSERT_EQ(result, 32);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 4);
    // 'C' - 'c' = -32.
    ASSERT_EQ(result, -32);
}

TEST(LlvmLibcStrNCmpTest,
     UnequalLengthStringsShouldNotReturnZeroWithSufficientLength) {
    const char *s1 = "abc";
    const char *s2 = "abcd";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 4);
    // '\0' - 'd' = -100.
    ASSERT_EQ(result, -100);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 4);
    // 'd' - '\0' = 100.
    ASSERT_EQ(result, 100);
}

TEST(LlvmLibcStrNCmpTest, StringArgumentSwapChangesSignWithSufficientLength) {
    const char *a = "a";
    const char *b = "b";
    int result = __STD_NAMESPACE::strncmp(b, a, 1);
    // 'b' - 'a' = 1.
    ASSERT_EQ(result, 1);

    result = __STD_NAMESPACE::strncmp(a, b, 1);
    // 'a' - 'b' = -1.
    ASSERT_EQ(result, -1);
}

// This group is actually testing strncmp functionality

TEST(LlvmLibcStrNCmpTest, NonEqualStringsEqualWithLengthZero) {
    const char *s1 = "abc";
    const char *s2 = "def";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 0);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 0);
    ASSERT_EQ(result, 0);
}

TEST(LlvmLibcStrNCmpTest, NonEqualStringsNotEqualWithLengthOne) {
    const char *s1 = "abc";
    const char *s2 = "def";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 1);
    ASSERT_EQ(result, -3);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 1);
    ASSERT_EQ(result, 3);
}

TEST(LlvmLibcStrNCmpTest, NonEqualStringsEqualWithShorterLength) {
    const char *s1 = "___B42__";
    const char *s2 = "___C55__";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 3);
    ASSERT_EQ(result, 0);

    // This should return 'B' - 'C' = -1.
    result = __STD_NAMESPACE::strncmp(s1, s2, 4);
    ASSERT_EQ(result, -1);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 3);
    ASSERT_EQ(result, 0);

    // This should return 'C' - 'B' = 1.
    result = __STD_NAMESPACE::strncmp(s2, s1, 4);
    ASSERT_EQ(result, 1);
}

TEST(LlvmLibcStrNCmpTest, StringComparisonEndsOnNullByteEvenWithLongerLength) {
    const char *s1 = "abc\0def";
    const char *s2 = "abc\0abc";
    int result = __STD_NAMESPACE::strncmp(s1, s2, 7);
    ASSERT_EQ(result, 0);

    // Verify operands reversed.
    result = __STD_NAMESPACE::strncmp(s2, s1, 7);
    ASSERT_EQ(result, 0);
}

//===-- Unittests for strncpy ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

void check_strncpy(std::vector<char> &dst,
                   const std::vector<char> &src, size_t n,
                   const std::vector<char> &expected) {
    // Making sure we don't overflow buffer.
    ASSERT_GE(dst.size(), n);
    // Making sure strncpy returns dst.
    ASSERT_EQ(__STD_NAMESPACE::strncpy(dst.data(), src.data(), n), dst.data());
    // Expected must be of the same size as dst.
    ASSERT_EQ(dst.size(), expected.size());
    // Expected and dst are the same.
    for (size_t i = 0; i < expected.size(); ++i)
        ASSERT_EQ(expected[i], dst[i]);
}

TEST(LlvmLibcStrncpyTest, Untouched) {
    std::vector<char> dst = {'a', 'b'};
    std::vector<char> src = {'x', '\0'};
    std::vector<char> expected = {'a', 'b'};
    check_strncpy(dst, src, 0, expected);
}

TEST(LlvmLibcStrncpyTest, CopyOne) {
    std::vector<char> dst = {'a', 'b'};
    std::vector<char> src = {'x', 'y'};
    std::vector<char> expected = {'x', 'b'}; // no \0 is appended
    check_strncpy(dst, src, 1, expected);
}

TEST(LlvmLibcStrncpyTest, CopyNull) {
    std::vector<char> dst = {'a', 'b'};
    std::vector<char> src = {'\0', 'y'};
    std::vector<char> expected = {'\0', 'b'};
    check_strncpy(dst, src, 1, expected);
}

TEST(LlvmLibcStrncpyTest, CopyPastSrc) {
    std::vector<char> dst = {'a', 'b'};
    std::vector<char> src = {'\0', 'y'};
    std::vector<char> expected = {'\0', '\0'};
    check_strncpy(dst, src, 2, expected);
}

//===-- Unittests for strnlen ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrNLenTest, EmptyString) {
    const char *empty = "";
    ASSERT_EQ(static_cast<size_t>(0), __STD_NAMESPACE::strnlen(empty, 0));
    // If N is greater than string length, this should still return 0.
    ASSERT_EQ(static_cast<size_t>(0), __STD_NAMESPACE::strnlen(empty, 1));
}

TEST(LlvmLibcStrNLenTest, OneCharacterString) {
    const char *single = "X";
    ASSERT_EQ(static_cast<size_t>(1), __STD_NAMESPACE::strnlen(single, 1));
    // If N is zero, this should return 0.
    ASSERT_EQ(static_cast<size_t>(0), __STD_NAMESPACE::strnlen(single, 0));
    // If N is greater than string length, this should still return 1.
    ASSERT_EQ(static_cast<size_t>(1), __STD_NAMESPACE::strnlen(single, 2));
}

TEST(LlvmLibcStrNLenTest, ManyCharacterString) {
    const char *many = "123456789";
    ASSERT_EQ(static_cast<size_t>(9), __STD_NAMESPACE::strnlen(many, 9));
    // If N is smaller than the string length, it should return N.
    ASSERT_EQ(static_cast<size_t>(3), __STD_NAMESPACE::strnlen(many, 3));
    // If N is zero, this should return 0.
    ASSERT_EQ(static_cast<size_t>(0), __STD_NAMESPACE::strnlen(many, 0));
    // If N is greater than the string length, this should still return 9.
    ASSERT_EQ(static_cast<size_t>(9), __STD_NAMESPACE::strnlen(many, 42));
}

TEST(LlvmLibcStrNLenTest, CharactersAfterNullTerminatorShouldNotBeIncluded) {
    const char str[5] = {'a', 'b', 'c', '\0', 'd'};
    ASSERT_EQ(static_cast<size_t>(3), __STD_NAMESPACE::strnlen(str, 3));
    // This should only read up to the null terminator.
    ASSERT_EQ(static_cast<size_t>(3), __STD_NAMESPACE::strnlen(str, 4));
    ASSERT_EQ(static_cast<size_t>(3), __STD_NAMESPACE::strnlen(str, 5));
}

//===-- Unittests for strpbrk ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrPBrkTest, EmptyStringShouldReturnNullptr) {
    // The search should not include the null terminator.
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk("", ""), nullptr);
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk("_", ""), nullptr);
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk("", "_"), nullptr);
}

TEST(LlvmLibcStrPBrkTest, ShouldNotFindAnythingAfterNullTerminator) {
    const char src[4] = {'a', 'b', '\0', 'c'};
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "c"), nullptr);
}

TEST(LlvmLibcStrPBrkTest, ShouldReturnNullptrIfNoCharactersFound) {
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk("12345", "abcdef"), nullptr);
}

TEST(LlvmLibcStrPBrkTest, FindsFirstCharacter) {
    const char *src = "12345";
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "1"), "12345");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "-1"), "12345");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "1_"), "12345");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "f1_"), "12345");
    ASSERT_STREQ(src, "12345");
}

TEST(LlvmLibcStrPBrkTest, FindsMiddleCharacter) {
    const char *src = "12345";
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "3"), "345");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "?3"), "345");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "3F"), "345");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "z3_"), "345");
    ASSERT_STREQ(src, "12345");
}

TEST(LlvmLibcStrPBrkTest, FindsLastCharacter) {
    const char *src = "12345";
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "5"), "5");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "r5"), "5");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "59"), "5");
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk(src, "n5_"), "5");
    ASSERT_STREQ(src, "12345");
}

TEST(LlvmLibcStrPBrkTest, FindsFirstOfRepeated) {
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk("A,B,C,D", ","), ",B,C,D");
}

TEST(LlvmLibcStrPBrkTest, FindsFirstInBreakset) {
    EXPECT_STREQ(__STD_NAMESPACE::strpbrk("12345", "34"), "345");
}

//===-- Unittests for strrchr ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrRChrTest, FindsFirstCharacter) {
    const char *src = "abcde";

    // Should return original string since 'a' is the first character.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr(src, 'a'), "abcde");
    // Source string should not change.
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrRChrTest, FindsMiddleCharacter) {
    const char *src = "abcde";

    // Should return characters after (and including) 'c'.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr(src, 'c'), "cde");
    // Source string should not change.
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrRChrTest, FindsLastCharacterThatIsNotNullTerminator) {
    const char *src = "abcde";

    // Should return 'e' and null-terminator.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr(src, 'e'), "e");
    // Source string should not change.
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrRChrTest, FindsNullTerminator) {
    const char *src = "abcde";

    // Should return null terminator.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr(src, '\0'), "");
    // Source string should not change.
    ASSERT_STREQ(src, "abcde");
}

TEST(LlvmLibcStrRChrTest, FindsLastBehindFirstNullTerminator) {
    const char src[6] = {'a', 'a', '\0', 'b', '\0', 'c'};
    // 'b' is behind a null terminator, so should not be found.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr(src, 'b'), nullptr);
    // Same goes for 'c'.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr(src, 'c'), nullptr);

    // Should find the second of the two a's.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr(src, 'a'), "a");
}

TEST(LlvmLibcStrRChrTest, CharacterNotWithinStringShouldReturnNullptr) {
    // Since 'z' is not within the string, should return nullptr.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr("123?", 'z'), nullptr);
}

TEST(LlvmLibcStrRChrTest, ShouldFindLastOfDuplicates) {
    // '1' is duplicated in the string, but it should find the last copy.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr("abc1def1ghi", '1'), "1ghi");

    const char *dups = "XXXXX";
    // Should return the last occurrence of 'X'.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr(dups, 'X'), "X");
}

TEST(LlvmLibcStrRChrTest, EmptyStringShouldOnlyMatchNullTerminator) {
    // Null terminator should match.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr("", '\0'), "");
    // All other characters should not match.
    ASSERT_STREQ(__STD_NAMESPACE::strrchr("", 'A'), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strrchr("", '2'), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strrchr("", '*'), nullptr);
}

//===-- Unittests for strspn ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrSpnTest, EmptyStringShouldReturnZeroLengthSpan) {
    // The search should not include the null terminator.
    EXPECT_EQ(__STD_NAMESPACE::strspn("", ""), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strspn("_", ""), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strspn("", "_"), size_t{0});
}

TEST(LlvmLibcStrSpnTest, ShouldNotSpanAnythingAfterNullTerminator) {
    const char src[4] = {'a', 'b', '\0', 'c'};
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "ab"), size_t{2});
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "c"), size_t{0});

    // Same goes for the segment to be searched for.
    const char segment[4] = {'1', '2', '\0', '3'};
    EXPECT_EQ(__STD_NAMESPACE::strspn("123", segment), size_t{2});
}

TEST(LlvmLibcStrSpnTest, SpanEachIndividualCharacter) {
    const char *src = "12345";
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "1"), size_t{1});
    // Since '1' is not within the segment, the span
    // size should remain zero.
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "2"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "3"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "4"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "5"), size_t{0});
}

TEST(LlvmLibcStrSpnTest, UnmatchedCharacterShouldNotBeCountedInSpan) {
    EXPECT_EQ(__STD_NAMESPACE::strspn("a", "b"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strspn("abcdef", "1"), size_t{0});
    EXPECT_EQ(__STD_NAMESPACE::strspn("123", "4"), size_t{0});
}

TEST(LlvmLibcStrSpnTest, SequentialCharactersShouldSpan) {
    const char *src = "abcde";
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "a"), size_t{1});
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "ab"), size_t{2});
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "abc"), size_t{3});
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "abcd"), size_t{4});
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "abcde"), size_t{5});
    // Same thing for when the roles are reversed.
    EXPECT_EQ(__STD_NAMESPACE::strspn("abcde", src), size_t{5});
    EXPECT_EQ(__STD_NAMESPACE::strspn("abcd", src), size_t{4});
    EXPECT_EQ(__STD_NAMESPACE::strspn("abc", src), size_t{3});
    EXPECT_EQ(__STD_NAMESPACE::strspn("ab", src), size_t{2});
    EXPECT_EQ(__STD_NAMESPACE::strspn("a", src), size_t{1});
}

TEST(LlvmLibcStrSpnTest, NonSequentialCharactersShouldNotSpan) {
    const char *src = "123456789";
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "_1_abc_2_def_3_"), size_t{3});
    // Only spans 4 since '5' is not within the span.
    EXPECT_EQ(__STD_NAMESPACE::strspn(src, "67__34abc12"), size_t{4});
}

TEST(LlvmLibcStrSpnTest, ReverseCharacters) {
    // Since these are still sequential, this should span.
    EXPECT_EQ(__STD_NAMESPACE::strspn("12345", "54321"), size_t{5});
    // Does not span any since '1' is not within the span.
    EXPECT_EQ(__STD_NAMESPACE::strspn("12345", "432"), size_t{0});
    // Only spans 1 since '2' is not within the span.
    EXPECT_EQ(__STD_NAMESPACE::strspn("12345", "51"), size_t{1});
}

TEST(LlvmLibcStrSpnTest, DuplicatedCharactersToBeSearchedForShouldStillMatch) {
    // Only a single character, so only spans 1.
    EXPECT_EQ(__STD_NAMESPACE::strspn("a", "aa"), size_t{1});
    // This should count once for each 'a' in the source string.
    EXPECT_EQ(__STD_NAMESPACE::strspn("aa", "aa"), size_t{2});
    EXPECT_EQ(__STD_NAMESPACE::strspn("aaa", "aa"), size_t{3});
    EXPECT_EQ(__STD_NAMESPACE::strspn("aaaa", "aa"), size_t{4});
}

//===-- Unittests for strstr ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrStrTest, NeedleNotInHaystack) {
    const char *haystack = "12345";
    const char *needle = "a";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), nullptr);
}

TEST(LlvmLibcStrStrTest, NeedleIsEmptyString) {
    const char *haystack = "12345";
    const char *needle = "";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), haystack);
}

TEST(LlvmLibcStrStrTest, HaystackIsEmptyString) {
    const char *haystack = "";
    const char *needle = "12345";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), nullptr);
}

TEST(LlvmLibcStrStrTest, HaystackAndNeedleAreEmptyStrings) {
    const char *haystack = "";
    const char *needle = "";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "");
}

TEST(LlvmLibcStrStrTest, HaystackAndNeedleAreSingleCharacters) {
    const char *haystack = "a";
    // Same characer returns that character.
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, /*needle=*/"a"), "a");
    // Different character returns nullptr.
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, /*needle=*/"b"), nullptr);
}

TEST(LlvmLibcStrStrTest, NeedleEqualToHaystack) {
    const char *haystack = "12345";
    const char *needle = "12345";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "12345");
}

TEST(LlvmLibcStrStrTest, NeedleSmallerThanHaystack) {
    const char *haystack = "12345";
    const char *needle = "345";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "345");
}

TEST(LlvmLibcStrStrTest, NeedleLargerThanHaystack) {
    const char *haystack = "123";
    const char *needle = "12345";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), nullptr);
}

TEST(LlvmLibcStrStrTest, NeedleAtBeginning) {
    const char *haystack = "12345";
    const char *needle = "12";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "12345");
}

TEST(LlvmLibcStrStrTest, NeedleInMiddle) {
    const char *haystack = "abcdefghi";
    const char *needle = "def";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "defghi");
}

TEST(LlvmLibcStrStrTest, NeedleDirectlyBeforeNullTerminator) {
    const char *haystack = "abcdefghi";
    const char *needle = "ghi";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "ghi");
}

TEST(LlvmLibcStrStrTest, NeedlePastNullTerminator) {
    const char haystack[5] = {'1', '2', '\0', '3', '4'};
    // Shouldn't find anything after the null terminator.
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, /*needle=*/"3"), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, /*needle=*/"4"), nullptr);
}

TEST(LlvmLibcStrStrTest, PartialNeedle) {
    const char *haystack = "la_ap_lap";
    const char *needle = "lap";
    // Shouldn't find la or ap.
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "lap");
}

TEST(LlvmLibcStrStrTest, MisspelledNeedle) {
    const char *haystack = "atalloftwocities...wait, tale";
    const char *needle = "tale";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "tale");
}

TEST(LlvmLibcStrStrTest, AnagramNeedle) {
    const char *haystack = "dgo_ogd_god_odg_gdo_dog";
    const char *needle = "dog";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, needle), "dog");
}

TEST(LlvmLibcStrStrTest, MorphedNeedle) {
    // Changes a single letter in the needle to mismatch with the haystack.
    const char *haystack = "once upon a time";
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, "time"), "time");
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, "lime"), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, "tome"), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, "tire"), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strstr(haystack, "timo"), nullptr);
}

//===-- Unittests for strtok_r -------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrTokReentrantTest, NoTokenFound) {
    { // Empty source and delimiter string.
        char empty[] = "";
        char *reserve = nullptr;
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(empty, "", &reserve), nullptr);
        // Another call to ensure that 'reserve' is not in a bad state.
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(empty, "", &reserve), nullptr);
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(nullptr, "", &reserve), nullptr);
    }
    { // Empty source and single character delimiter string.
        char empty[] = "";
        char *reserve = nullptr;
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(empty, "_", &reserve), nullptr);
        // Another call to ensure that 'reserve' is not in a bad state.
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(empty, "_", &reserve), nullptr);
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(nullptr, "_", &reserve), nullptr);
    }
    { // Same character source and delimiter string.
        char single[] = "_";
        char *reserve = nullptr;
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(single, "_", &reserve), nullptr);
        // Another call to ensure that 'reserve' is not in a bad state.
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(single, "_", &reserve), nullptr);
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(nullptr, "_", &reserve), nullptr);
    }
    { // Multiple character source and single character delimiter string.
        char multiple[] = "1,2";
        char *reserve = nullptr;
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(multiple, ":", &reserve), "1,2");
        // Another call to ensure that 'reserve' is not in a bad state.
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(multiple, ":", &reserve), "1,2");
        ASSERT_STREQ(__STD_NAMESPACE::strtok_r(nullptr, ":", &reserve), nullptr);
    }
}

TEST(LlvmLibcStrTokReentrantTest, DelimiterAsFirstCharacterShouldBeIgnored) {
    char src[] = ".123";
    char *reserve = nullptr;
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ".", &reserve), "123");
    // Another call to ensure that 'reserve' is not in a bad state.
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ".", &reserve), "123");
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(nullptr, ".", &reserve), nullptr);
}

TEST(LlvmLibcStrTokReentrantTest, DelimiterIsMiddleCharacter) {
    char src[] = "12,34";
    char *reserve = nullptr;
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ",", &reserve), "12");
    // Another call to ensure that 'reserve' is not in a bad state.
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ",", &reserve), "12");
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(nullptr, ",", &reserve), nullptr);
}

TEST(LlvmLibcStrTokReentrantTest, DelimiterAsLastCharacterShouldBeIgnored) {
    char src[] = "1234:";
    char *reserve = nullptr;
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ":", &reserve), "1234");
    // Another call to ensure that 'reserve' is not in a bad state.
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ":", &reserve), "1234");
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(nullptr, ":", &reserve), nullptr);
}

TEST(LlvmLibcStrTokReentrantTest, ShouldNotGoPastNullTerminator) {
    char src[] = {'1', '2', '\0', ',', '3'};
    char *reserve = nullptr;
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ",", &reserve), "12");
    // Another call to ensure that 'reserve' is not in a bad state.
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ",", &reserve), "12");
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(nullptr, ",", &reserve), nullptr);
}

TEST(LlvmLibcStrTokReentrantTest,
     ShouldReturnNullptrWhenBothSrcAndSaveptrAreNull) {
    char *src = nullptr;
    char *reserve = nullptr;
    // Ensure that instead of crashing if src and reserve are null, nullptr is
    // returned
    ASSERT_STREQ(__STD_NAMESPACE::strtok_r(src, ",", &reserve), nullptr);
    // And that neither src nor reserve are changed when that happens
    ASSERT_STREQ(src, nullptr);
    ASSERT_STREQ(reserve, nullptr);
}

TEST(LlvmLibcStrTokReentrantTest,
     SubsequentCallsShouldFindFollowingDelimiters) {
    char src[] = "12,34.56";
    char *reserve = nullptr;
    char *token = __STD_NAMESPACE::strtok_r(src, ",.", &reserve);
    ASSERT_STREQ(token, "12");
    token = __STD_NAMESPACE::strtok_r(nullptr, ",.", &reserve);
    ASSERT_STREQ(token, "34");
    token = __STD_NAMESPACE::strtok_r(nullptr, ",.", &reserve);
    ASSERT_STREQ(token, "56");
    token = __STD_NAMESPACE::strtok_r(nullptr, "_:,_", &reserve);
    ASSERT_STREQ(token, nullptr);
    // Subsequent calls after hitting the end of the string should also return
    // nullptr.
    token = __STD_NAMESPACE::strtok_r(nullptr, "_:,_", &reserve);
    ASSERT_STREQ(token, nullptr);
}

TEST(LlvmLibcStrTokReentrantTest, DelimitersShouldNotBeIncludedInToken) {
    char src[] = "__ab__:_cd__:__ef__:__";
    char *reserve = nullptr;
    char *token = __STD_NAMESPACE::strtok_r(src, "_:", &reserve);
    ASSERT_STREQ(token, "ab");
    token = __STD_NAMESPACE::strtok_r(nullptr, ":_", &reserve);
    ASSERT_STREQ(token, "cd");
    token = __STD_NAMESPACE::strtok_r(nullptr, "_:,", &reserve);
    ASSERT_STREQ(token, "ef");
    token = __STD_NAMESPACE::strtok_r(nullptr, "_:,_", &reserve);
    ASSERT_STREQ(token, nullptr);
}

//===-- Unittests for strtok ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

TEST(LlvmLibcStrTokTest, NoTokenFound) {
    char empty[] = "";
    ASSERT_STREQ(__STD_NAMESPACE::strtok(empty, ""), nullptr);
    ASSERT_STREQ(__STD_NAMESPACE::strtok(empty, "_"), nullptr);

    char single[] = "_";
    ASSERT_STREQ(__STD_NAMESPACE::strtok(single, ""), "_");

    char multiple[] = "1,2";
    ASSERT_STREQ(__STD_NAMESPACE::strtok(multiple, ":"), "1,2");
}

TEST(LlvmLibcStrTokTest, DelimiterAsFirstCharacterShouldBeIgnored) {
    char src[] = ".123";
    ASSERT_STREQ(__STD_NAMESPACE::strtok(src, "."), "123");
}

TEST(LlvmLibcStrTokTest, DelimiterIsMiddleCharacter) {
    char src[] = "12,34";
    ASSERT_STREQ(__STD_NAMESPACE::strtok(src, ","), "12");
}

TEST(LlvmLibcStrTokTest, DelimiterAsLastCharacterShouldBeIgnored) {
    char src[] = "1234:";
    ASSERT_STREQ(__STD_NAMESPACE::strtok(src, ":"), "1234");
}

TEST(LlvmLibcStrTokTest, MultipleDelimiters) {
    char src[] = "12,.34";
    ASSERT_STREQ(__STD_NAMESPACE::strtok(src, "."), "12,");
    ASSERT_STREQ(__STD_NAMESPACE::strtok(src, ".,"), "12");
    ASSERT_STREQ(__STD_NAMESPACE::strtok(src, ",."), "12");
    ASSERT_STREQ(__STD_NAMESPACE::strtok(src, ":,."), "12");
}

TEST(LlvmLibcStrTokTest, ShouldNotGoPastNullTerminator) {
    char src[] = {'1', '2', '\0', ',', '3'};
    ASSERT_STREQ(__STD_NAMESPACE::strtok(src, ","), "12");
}

TEST(LlvmLibcStrTokTest, SubsequentCallsShouldFindFollowingDelimiters) {
    char src[] = "12,34.56";
    char *token = __STD_NAMESPACE::strtok(src, ",.");
    ASSERT_STREQ(token, "12");
    token = __STD_NAMESPACE::strtok(nullptr, ",.");
    ASSERT_STREQ(token, "34");
    token = __STD_NAMESPACE::strtok(nullptr, ",.");
    ASSERT_STREQ(token, "56");
    token = __STD_NAMESPACE::strtok(nullptr, "_:,_");
    ASSERT_STREQ(token, nullptr);
    // Subsequent calls after hitting the end of the string should also return
    // nullptr.
    token = __STD_NAMESPACE::strtok(nullptr, "_:,_");
    ASSERT_STREQ(token, nullptr);
}

TEST(LlvmLibcStrTokTest, DelimitersShouldNotBeIncludedInToken) {
    char src[] = "__ab__:_cd__:__ef__:__";
    char *token = __STD_NAMESPACE::strtok(src, "_:");
    ASSERT_STREQ(token, "ab");
    token = __STD_NAMESPACE::strtok(nullptr, ":_");
    ASSERT_STREQ(token, "cd");
    token = __STD_NAMESPACE::strtok(nullptr, "_:,");
    ASSERT_STREQ(token, "ef");
    token = __STD_NAMESPACE::strtok(nullptr, "_:,_");
    ASSERT_STREQ(token, nullptr);
}

namespace strerrorTest {
const char *kErrorStrings[] = {
    "Success",
    "Operation not permitted",
    "No such file or directory",
    "No such process",
    "Interrupted system call",
    "Input/output error",
    "No such device or address",
    "Argument list too long",
    "Exec format error",
    "Bad file descriptor",
    "No child processes",
    "Resource temporarily unavailable",
    "Cannot allocate memory",
    "Permission denied",
    "Bad address",
    "Block device required",
    "Device or resource busy",
    "File exists",
    "Invalid cross-device link",
    "No such device",
    "Not a directory",
    "Is a directory",
    "Invalid argument",
    "Too many open files in system",
    "Too many open files",
    "Inappropriate ioctl for device",
    "Text file busy",
    "File too large",
    "No space left on device",
    "Illegal seek",
    "Read-only file system",
    "Too many links",
    "Broken pipe",
    "Numerical argument out of domain",
    "Numerical result out of range",
    "Resource deadlock avoided",
    "File name too long",
    "No locks available",
    "Function not implemented",
    "Directory not empty",
    "Too many levels of symbolic links",
    "Resource temporarily unavailable",
    "No message of desired type",
    "Identifier removed",
    "Channel number out of range",
    "Level 2 not synchronized",
    "Level 3 halted",
    "Level 3 reset",
    "Link number out of range",
    "Protocol driver not attached",
    "No CSI structure available",
    "Level 2 halted",
    "Invalid exchange",
    "Invalid request descriptor",
    "Exchange full",
    "No anode",
    "Invalid request code",
    "Invalid slot",
    "Resource deadlock avoided",
    "Bad font file format",
    "Device not a stream",
    "No data available",
    "Timer expired",
    "Out of streams resources",
    "Machine is not on the network",
    "Package not installed",
    "Object is remote",
    "Link has been severed",
    "Advertise error",
    "Srmount error",
    "Communication error on send",
    "Protocol error",
    "Multihop attempted",
    "RFS specific error",
    "Bad message",
    "Value too large for defined data type",
    "Name not unique on network",
    "File descriptor in bad state",
    "Remote address changed",
    "Can not access a needed shared library",
    "Accessing a corrupted shared library",
    ".lib section in a.out corrupted",
    "Attempting to link in too many shared libraries",
    "Cannot exec a shared library directly",
    "Invalid or incomplete multibyte or wide character",
    "Interrupted system call should be restarted",
    "Streams pipe error",
    "Too many users",
    "Socket operation on non-socket",
    "Destination address required",
    "Message too long",
    "Protocol wrong type for socket",
    "Protocol not available",
    "Protocol not supported",
    "Socket type not supported",
    "Operation not supported",
    "Protocol family not supported",
    "Address family not supported by protocol",
    "Address already in use",
    "Cannot assign requested address",
    "Network is down",
    "Network is unreachable",
    "Network dropped connection on reset",
    "Software caused connection abort",
    "Connection reset by peer",
    "No buffer space available",
    "Transport endpoint is already connected",
    "Transport endpoint is not connected",
    "Cannot send after transport endpoint shutdown",
    "Too many references: cannot splice",
    "Connection timed out",
    "Connection refused",
    "Host is down",
    "No route to host",
    "Operation already in progress",
    "Operation now in progress",
    "Stale file handle",
    "Structure needs cleaning",
    "Not a XENIX named type file",
    "No XENIX semaphores available",
    "Is a named type file",
    "Remote I/O error",
    "Disk quota exceeded",
    "No medium found",
    "Wrong medium type",
    "Operation canceled",
    "Required key not available",
    "Key has expired",
    "Key has been revoked",
    "Key was rejected by service",
    "Owner died",
    "State not recoverable",
    "Operation not possible due to RF-kill",
    "Memory page has hardware error",
    "Operation not supported",
};
}
TEST(LlvmLibcStrerrorTest, CompareMessages) {

    for (int i = 0; i <= ENOTSUP; i++) {
        EXPECT_STREQ(strerrorTest::kErrorStrings[i], __STD_NAMESPACE::strerror(i));
    }
}