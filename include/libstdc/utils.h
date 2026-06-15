#ifndef STDLIBC_UTILS_H
#define STDLIBC_UTILS_H

#define max(a, b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a, b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#endif //STDLIBC_UTILS_H
