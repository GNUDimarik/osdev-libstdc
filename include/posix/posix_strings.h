#ifndef POSIX_STRINGS_H
#define POSIX_STRINGS_H

#include <stddef.h>
#include "config.h"
#include "cstring.h"

__BEGIN_STDLIB_TEST_NAMESPACE

/**
 * @ingroup strings
 * @{
 */

/**
 * @brief bcmp  - just macro for calling memcmp
 * @see memcmp
 */
#if defined(__POSIX_STRINGS_MACRO__)
#define bcmp(b1, b2, len) memcmp((b1), (b2), (size_t)(len))
#else
static inline int bcmp(const void *b1, const void *b2, size_t len)
{
    return __STD_NAMESPACE::memcmp(b1, b2, len);
}
#endif

/**
 * @brief bzero - just macro for calling memset
 * @see memset
 */
#if defined(__POSIX_STRINGS_MACRO__)
#define bzero(b, len) (memset((b), '\0', (len)), (void) 0)
#else
static inline void bzero(void *b, size_t len)
{
    __STD_NAMESPACE::memset(b, 0, len);
}
#endif
/**
 * @brief index - just macro for calling strchr
 * @see strchr
 */
#if defined(__POSIX_STRINGS_MACRO__)
#define index(a, b) strchr((a),(b))
#else
static inline char *index(const char *a, int b)
{
    return __STD_NAMESPACE::strchr(a, b);
}
#endif

/**
 * @brief rindex - just macro for calling strrchr
 */
#if defined(__POSIX_STRINGS_MACRO__)
#define rindex(a, b) strrchr((a),(b))
#else
static inline char *rindex(const char *a, int b)
{
    return __STD_NAMESPACE::strrchr(a, b);
}
#endif

/**
 * @brief bcopy   - just macro for calling memmove
 * @see memmove
 */
#if defined(__POSIX_STRINGS_MACRO__)
#define bcopy(b1, b2, len) (memmove((b2), (b1), (len)), (void) 0)
#else
static inline void *bcopy(void *b1, const void *b2, size_t len)
{
    return __STD_NAMESPACE::memmove(b1, b2, len);
}
#endif

/**
 * @brief strlcpy - copies not more than size bytes from src to dest and guaranteed append NUL byte at end
 * @param dst     - destination of copy
 * @param src     - destination copy to
 * @param size    - max size of bytes to copy
 * @return        - length of source
 * @see strncpy
 */
size_t
strlcpy(char *__restrict__ dst, const char *__restrict__ src, size_t size);

/**
 * @brief strlcat -  appends string src to the end of dst. It will append at most dstsize – strlen(dst) – 1 characters.
 *                   It will then NUL-terminate, unless dstsize is 0 or the original dst string was longer than dstsize
 *                   (in practice this should not happen as it means that either dstsize is incorrect or that
 *                   dst is not a proper string).
 * @param dst      - destination buffer
 * @param src      - string to be appended to dst
 * @param dstsize  -  destination buffer size
 * @return the total length of the string it tried to create
 *
 * @see strcat, strncat
 */
size_t strlcat(char *__restrict__ dst, const char *__restrict__ src, size_t dstsize);

/**
 * @brief strcasecmp - function performs a byte-by-byte comparison of the strings first and second, ignoring the case
 *                     of the characters.  It returns an integer less than, equal to, or greater than zero if first is
 *                     found, respectively, to be less than, to match, or be greater than s2.
 * @param first       - first string to compare
 * @param second      - second string to compare
 * @return  an integer less than, equal to, or greater than zero if first is, after ignoring case, found to be less than,
 *          to match, or be greater  than  second,  respectively.
 * @see strcmp
 */
int strcasecmp(const char *first, const char *second);

/**
 * @brief strncasecmp - function performs a byte-by-byte comparison of the strings first and second, ignoring the case
 *                      of the characters.  It returns an integer less than, equal to, or greater than zero if first is
 *                      found, respectively, to be less than, to match, or be greater than second
 * @param first       - first string to compare
 * @param second      - second string to compare
 * @param n           - number of characters to compare
 * @return  an integer less than, equal to, or greater than zero if first is, after ignoring case, found to be less than,
 *          to match, or be greater  than  second,  respectively.
 */
int strncasecmp(const char *first, const char *second, size_t n);

/**
 * @brief  ffs - find first set bit
 * @param val  - integer number
 * @return the position of the first (least significant) bit set in the number i
 */
int ffs(int val);

/**
 * @brief ffsl - find first set bit
 * @param val  - long number
 * @return the position of the first (least significant) bit set in the number i
 */
int ffsl(long int val);

/**
 * @brief ffsll - find first set bit
 * @param val   - long long number
 * @return the position of the first (least significant) bit set in the number i
 */
int ffsll(long long int val);

__END_STDLIB_TEST_NAMESPACE

#endif //POSIX_STRINGS_H
