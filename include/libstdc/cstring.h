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

#ifndef STDLIBC_CSTRING_H
#define STDLIBC_CSTRING_H

#include "config.h"
#include <stddef.h>

__MAYBE_BEGIN_STD_NAMESPACE
__BEGIN_DECLS

/**
 * @defgroup strings string routines
 * @ingroup  stdlib
 * @{
 */

/**
 * @brief memcpy - copy length bytes from the object pointed to by src into
 *                 the object pointed to by dest. If copying takes place between
 *                 objects that overlap, the behavior is undefined.
 * @param dest   - pointer to destination
 * @param src    - pointer to source
 * @param length - bytes number to copy
 * @return       - pointer to destination
 *
 * The memcpy() function does not check for the overflow of the receiving
 * memory area.
 */
void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t length);

/**
 * @brief memmove - copies length bytes from the object pointed to by src into the object
 *                  pointed to by dest.
 * @param dest    - pointer to area to copy to
 * @param src     - pointer to area to copy from
 * @param length  - bytes number to copy
 * @return        - dest
 *
 * Case when src less than dest handles correctly.
 * There is no temporary array inside
 */
void *memmove(void *dest, const void *src, size_t length);

/**
 * @brief  memchr - locate the first occurrence of needle
 *                  (converted to an unsigned char) in the initial count bytes
 *                  (each interpreted as unsigned char) pointed to by memory.
 * @param  memory - memory where we want to search needle
 * @param  needle - characted to search in memory
 * @param  count  - count of bytes for searching
 * @return          pointer to the located byte, or a null pointer
 *                  if the byte is not found.
 */
void *memchr(const void *memory, int needle, size_t count);

/**
 * @brief memchr -
 * @param mem
 * @param needle
 * @param count
 * @return
 */
void *memrchr(const void *mem, int needle, size_t count);
/**
 * @brief memcmp - compares length bytes (each interpreted as unsigned char)
 *                 the object pointed to by first to the first n bytes of the
 *                 object pointed to by second.
 * @param first  - pointer to first memory area
 * @param second - pointer to second memory area
 * @param length - bytes number to compare
 * @return         -1 if first < second, 1 otherwise and 0 if they are equal
 */
int memcmp(const void *first, const void *second, size_t length);

/**
 * @brief  memset - copies valie (converted to an unsigned char) into each
 *                  of the first length bytes of the object pointed to by dest.
 * @param  dest   - dest to set
 * @param  value  - value to set which
 * @param  length - size of bytes to fill
 * @return          dest; no return value is reserved to indicate an error.
 */
void *memset(void *dest, int value, size_t length);

/**
 * @brief  strcat - append a copy of the string pointed to by src
 *                  (including the terminating NUL character) to the end of the
 *                  string pointed to by dest.
 * @param  dest   - string to append to
 * @param  src    - string to append which
 * @return        - dest. no return value shall be reserved to indicate an
 * error.
 *
 * The initial byte of src overwrites the NUL character at the end of dest.
 * If copying takes place between objects that overlap, the behavior
 * is undefined.
 */
char *strcat(char *__restrict__ dest, const char *__restrict__ src);

/**
 * @brief  strncat  - append not more than length bytes (a NUL character and
 *                    bytes that follow it are not appended) from the array
 *                    pointed to by src to the end of the string pointed to by
 *                    dest
 * @param  dest     - string to append to
 * @param  src      - string to append which
 * @param length    - number characters to concatenate
 * @return            dest. no return value shall be reserved to indicate an
 *                    error.
 */
char *strncat(char *dest, const char *src, size_t length);

/**
 * @brief  strchr - locates the first occurrence of needle (converted to a char)
 *                  in the string pointed to by str. The terminating NUL
 * character is considered to be part of the string
 * @param  str    - string to search to
 * @param  needle - character to search which
 * @return          a pointer to the byte, or a null pointer if the byte was
 *                  not found
 */
char *strchr(const char *str, int needle);

/**
 * @brief  strrchr - locates the last occurrence of needle (converted to a char)
 *                   in the string pointed to by str. The terminating NUL
 *                   character is considered to be part of the string.
 * @param  str     - string to search to
 * @param  needle  - character to search which
 * @return           a pointer to the byte or a null pointer if needle does not
 *                   occur in the string.
 */
char *strrchr(const char *str, int needle);

/**
 * @brief  strcmp - ompare the string pointed to by first to the string
 *         pointed to by second
 * @param  first  - pointer to first string
 * @param  second - pointer to second string
 * @return -1 if first < second, 1 otherwise and 0 if they are equal
 */
int strcmp(const char *first, const char *second);

/**
 * @brief  strncmp - compare not more than len bytes (bytes that follow a NUL
 *         character are not compared) from the array pointed to by first to
 *         the array pointed to by second
 * @param  first   - pointer to first string
 * @param  second  - pointer to second string
 * @param  length  - bytes number to compare
 * @return           -1 if first < second, 1 otherwise and 0 if they are equal
 */
int strncmp(const char *first, const char *second, size_t length);

/**
 * @brief  strcpy - copies the string pointed to by src (including the
 *                  terminating NUL character) into the array pointed to by dest
 * @param  dest   - destination of copy
 * @param  src    - source of copy
 * @return          a pointer  to dest
 *
 * If copying takes place between objects that overlap, the behavior is
 * undefined
 */
char *strcpy(char *dest, const char *src);

/**
 * @brief strncpy - not more than n bytes (bytes that follow a NULL character
 *        are not copied) from the array pointed to by src to the array
 *        pointed to by dest
 *
 * @note If copying takes place between objects that overlap, the behavior is undefined.
 *
 * If the array pointed to by dest is a string that is shorter than n characters, null characters
 * are appended to the copy in the array pointed to by src, until n characters in all have been
 * written
 *
 * @param dest    - destination of copying
 * @param src     - source of copying
 * @param n       - number bytes to copy
 * @return        - a value of dest
 */
char *strncpy(char *dest, const char *src, size_t n);

/**
 * @brief  strspn - computes the length (in bytes) of the maximum initial
 * segment of the string pointed to by str which consists entirely of bytes from
 * the string pointed to by accept
 * @param  str    - string to check
 * @param  accept - set of accepted characters
 * @return          the computed length; no return value is reserved to indicate
 *                  an error
 */
size_t strspn(const char *str, const char *accept);

/**
 * @see str strnlen
 * @brief  strlen - compute the number of bytes in the string to which str
 * points, not including the terminating NUL character.
 * @param  str    - string to calculate length
 * @return          length of str
 */
size_t strlen(const char *str);

/**
 * @see strlen
 * @brief strnlen - returns the number of bytes in the string
       pointed to by s, excluding the terminating null byte ('\0'), but
       at most maxlen.  In doing this, strnlen() looks only at the first
       maxlen characters in the string pointed to by s and never beyond
       s[maxlen-1].
 * @param str      - string to calculate length
 * @param max_len  - value to return if strlen(str) above than it
 * @return strlen(s), if that is less than
           maxlen, or maxlen if there is no null terminating ('\0') among
           the first maxlen characters pointed
 */
size_t strnlen(const char *str, size_t max_len);

/**
 * @brief strcspn - compute the length (in bytes) of the maximum initial segment
 *                  of the string pointed to by str which consists entirely of
 *                  bytes not from the string pointed to by reject
 * @param str     - string to check
 * @param reject  - set of rejected characters
 * @return        - index of first occurency of any character from reject or
 *                  NULL if not found no return value is reserved to indicate an
 *                  error.
 */
size_t strcspn(const char *str, const char *reject);

/**
 * @brief  strpbrk - locate the first occurrence in the string pointed to by str
 *                   of any byte from the string pointed to by accept
 * @param  str     - string to check
 * @param  accept  - set of accepted characters
 * @return         - a pointer to the byte or a null pointer if no byte from str
 *                   occurs in accept
 */
char *strpbrk(const char *str, const char *accept);

/**
 * @brief  strstr   - locate the first occurrence in the string pointed
 *                    to by haystack of the sequence of bytes
 *                    (excluding the terminating null byte) in the string
 *                    pointed to by needle
 * @param  haystack - string to search in
 * @param  needle   - string which needs to be find
 * @return            a pointer to the located string or a null pointer
 *                    if the string is not found
 *
 * If needle points to a string with zero length, the function shall return
 * haystack.
 */
char *strstr(const char *haystack, const char *needle);

/**
 * @brief  strtok    - a sequence of calls to strtok_r() breaks the string pointed to
 *                    by str into a sequence of tokens, each of which is delimited
 *                    by a byte from the string pointed to by delim
 * @param  str       - string to split
 * @param  delim     - separators
 * @return           - pointer to the first byte of a token. Otherwise, if there
 *                     is no token, strtok() shall return a null pointer.
 *
 * the first call in the sequence has s as its first argument, and is followed
 * by calls with a null pointer as their first argument. The separator string
 * pointed to by sep may be different from call to call.
 *
 * The first call in the sequence searches the string pointed to by str for the
 * first byte that is not contained in the current separator string pointed to
 * by delim. If no such byte is found, then there are no tokens in the string
 * pointed to by str and strtok() shall return a null pointer. If such a byte
 * is found, it is the start of the first token.
 *
 * The strtok() function then searches from there for a byte that is contained
 * in the current separator string. If no such byte is found, the current token
 * extends to the end of the string pointed to by str, and subsequent searches
 * for a token shall return a null pointer. If such a byte is found, it is
 * overwritten by a NUL character, which terminates the current token.
 * The strtok() function saves a pointer to the following byte, from which the
 * next search for a token shall start.
 *
 * Each subsequent call, with a null pointer as the value of the first argument,
 * starts searching from the saved pointer and behaves as described above.
 */
char *strtok(char *str, const char *delim);

/**
 * @see strtock
 * @brief strtok_r - is a reentrant version of strtok
 * @param str      - string to split
 * @param delim    - separators
 * @param saveptr  - used internally in order to maintain context between successive calls
 * @return
 */
char *strtok_r(char *str, const char *delim, char **saveptr);

/**
 * @brief memccpy - copies not more than n bytes from src to dest and stop working once value of c occurred
 * @param dest    - destination of the copy
 * @param src     - source of the copy
 * @param c       - stop character
 * @param n       - number of bytest to copy
 * @return        - pointer to pos after c in dest or NULL if no c occurred
 */
void *memccpy(void *dest, const void *src, int c, size_t n);

/**
 * @brief strerror - returns string representation of errno value
 * @param errnum   - errno number
 * @return         -
 */
char *strerror(int errnum);

/**
 * @brief strrev - reverses input string
 * @param str    - input string to reverse
 * @return       - reversed version of str
 * @note strrev implements inplace algorithm. e.g. it modifies an input string
 */
char *strrev(char *str);

/**
 * @brief  strupr - is used for converting of string to upper case
 * @param str     - string to be upper cased
 * @return upper cased version of str
 * @note strupr implements inplace algorithm. e.g. it modifies an input string
 */
char *strupr(char *str);

/**
 * @brief  strlwr - is used for converting of string to lower case
 * @param str     - string to be lower cased
 * @return upper cased version of str
 * @note strlwr implements inplace algorithm. e.g. it modifies an input string
 */
char *strlwr(char *str);

/**
 * @brief The mempcpy() function is nearly identical to the memcpy(3) function.
 * It copies n bytes from the object beginning at src into the object pointed to by dest.
 * But instead of returning the value of dest it returns a pointer to the byte following the last written byte.
 *
 * @param dest   - pointer to destination
 * @param src    - pointer to source
 * @param n      - bytes number to copy
 *
 * @return dest + n position
 *
 * @see memcpy
 */
void *mempcpy(void *dest, const void *src, size_t n);

/**
 * @brief stpcpy  - function copies the string pointed to by src (including the terminating null byte ('\0'))
 *                  to the array pointed to by dest. The strings may not overlap, and the destination string dest
 *                  must be large enough to receive the copy
 * @param dest    - destination string
 * @param src     - source string
 * @return a pointer to the end of the string dest (that is, the address of the terminating null byte)
 * rather than the beginning
 *
 * @see strcpy
 */
char *stpcpy(char *dest, const char *src);

__END_DECLS
__MAYBE_END_STD_NAMESPACE
/** @} */
#endif /* STDLIBC_CSTRING_H */