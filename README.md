# osdev-libstdc

![License](https://img.shields.io/badge/license-MIT-green)
![Compiler](https://img.shields.io/badge/GCC-16.1+-blue)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue)
![Platform](https://img.shields.io/badge/platform-freestanding-orange)

A lightweight freestanding C/C++ library with zero external dependencies designed for operating system development.

`osdev-libstdc` provides a practical subset of commonly used C and C++ runtime functionality suitable for kernels, bootloaders, embedded systems, and standalone runtimes.

The project is a part of the **DUX Operating System** and is intended to work without any dependency on a hosted libc implementation.

---

## Highlights

* Freestanding compatible
* No dependency on host libc
* Thread-safe boundary-tag allocator
* Segregated free lists
* Support for aligned allocations
* Lightweight `printf` implementation
* POSIX string extensions
* Character classification routines
* Kernel-friendly spin lock
* RAII lock management
* Portable across 32-bit and 64-bit targets

---

# Implemented Components

## C Library

| Header      | Status |
| ----------- | ------ |
| `printf.h`  | ✓      |
| `malloc.h`  | ✓      |
| `atomic.h`  | ✓      |
| `string.h`  | ✓      |
| `strings.h` | ✓      |
| `ctype.h`   | ✓      |

## C++ Library

| Component         | Status |
| ----------------- | ------ |
| `dux::spin_lock`  | ✓      |
| `std::lock_guard` | ✓      |

---

# Formatted Output (`printf.h`)

```c
#include <printf.h>
```

The library provides a lightweight freestanding implementation of the `printf` family suitable for kernels, bootloaders, runtime libraries, and embedded environments.

## Format Grammar

```text
%[flags][field_width][.precision][length]specifier
```

## Supported Format Specifiers

`%d`, `%i`, `%u`, `%o`, `%x`, `%X`, `%p`, `%c`, `%s`, `%n`, `%%`

## Supported Flags

`- + <space> # 0`

## Supported Length Modifiers

`hh`, `h`, `l`, `ll`, `z`, `t`

## Pointer Formatting

Null pointers are formatted as:

```text
%p (nil) %s (null)
```

## Implemented Functions

```c
int vsnprintf(char *buf, size_t max_size, const char *fmt, va_list ap);
int snprintf(char *buffer, size_t size, const char *fmt, ...);
int sprintf(char *buffer, const char *fmt, ...);
```

---

# Memory Allocation (`malloc.h`)

![Allocator Overview](https://raw.githubusercontent.com/GNUDimarik/small_allocator/main/docs/images/allocator-overview.png)

```c
#include <malloc.h>
```

The allocator is based on an implicit free list with boundary tags and segregated free lists.

## Implemented Functions

```c
void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t new_size);
void free(void *ptr);
void *aligned_alloc(size_t alignment, size_t size);
```

## Features

* Thread-safe
* Boundary tag allocator
* Segregated free lists
* Adjacent block coalescing
* Support for aligned allocations
* Corruption detection using magic values
* Constant-time free-list insertion
* Suitable for kernel and runtime use

## Allocator Design

```text
Bin[32]
Bin[64]
Bin[96]
...
Bin[254]
Large Bin (>= 255 bytes)
```

Small blocks are stored in dedicated size-class bins.

Large blocks are maintained in a sorted large-block list.

## Memory Block Layout

![Block Layout](https://raw.githubusercontent.com/GNUDimarik/small_allocator/main/docs/images/block-overview.png)

Each memory block contains:

* Header
* Footer
* Block size
* Allocation state
* Integrity verification values

## Free Block Layout

![Free Block Layout](https://raw.githubusercontent.com/GNUDimarik/small_allocator/main/docs/images/free-block-overview.png)

Free blocks reuse their payload area to store free-list links.

## Aligned Allocation Block

![Aligned Allocation Block](https://raw.githubusercontent.com/GNUDimarik/small_allocator/main/docs/images/aligned-block-overview.png)

Aligned allocations support arbitrary power-of-two alignments.

## Thread Safety

The allocator is internally synchronized and safe for concurrent use.

Protected APIs:

```text
malloc
calloc
realloc
free
aligned_alloc
```

## Error Handling

```text
malloc(0)         -> nullptr
calloc overflow   -> nullptr
allocation failed -> nullptr
```

---

# Atomic Operations (`atomic.h`)

```cpp
#include <atomic.h>
```

The library provides a lightweight freestanding implementation of `std::atomic` based on GCC `__atomic` builtins.

## Implemented Constants

`hardware_destructive_interference_size`, `hardware_constructive_interference_size`

## Supported Memory Orders

`memory_order_relaxed`, `memory_order_acquire`, `memory_order_release`, `memory_order_acq_rel`, `memory_order_seq_cst`

## Implemented Type

`std::atomic<T>`

## Implemented Operations

`operator=`, `is_lock_free`, `store`, `load`, `operator T`, `exchange`, `compare_exchange_weak`, `compare_exchange_strong`

## Features

* Freestanding compatible
* No operating system dependencies
* Based on GCC `__atomic` builtins
* Supports explicit memory ordering
* Provides volatile and non-volatile overloads
* Suitable for low-level synchronization primitives
* Used as a foundation for `dux::spin_lock`

## Notes

`memory_order_consume` is intentionally not implemented.

# Concurrency (`dux/spin_lock.h`)

```cpp
#include <dux/spin_lock.h>
```

The library provides a lightweight spin lock implementation suitable for kernels, runtime libraries, and freestanding environments.

## Implemented Types

* `dux::spin_lock`
* `std::lock_guard<T>`

## Spin Lock API

```cpp
lock.lock();
lock.unlock();
lock.try_lock();
```

## Features

* Freestanding compatible
* No operating system dependencies
* Atomic-operation based implementation
* Low-overhead synchronization primitive
* Suitable for kernels and runtimes
* Suitable for allocator synchronization

---

# String and Memory Routines (`string.h`)

```c
#include <string.h>
```

## Memory Manipulation

`memcpy`, `memmove`, `memset`, `memcmp`, `memchr`, `memrchr`, `memccpy`, `mempcpy`

## String Manipulation

`strcpy`, `strncpy`, `stpcpy`, `strcat`, `strncat`, `strlen`, `strnlen`, `strrev`, `strupr`, `strlwr`

## String Comparison

`strcmp`, `strncmp`

## String Search

`strchr`, `strrchr`, `strstr`, `strpbrk`, `strspn`, `strcspn`

## Tokenization

`strtok`, `strtok_r`

## Miscellaneous

`strerror`

---

# POSIX Extensions (`strings.h`)

```c
#include <strings.h>
```

## BSD / POSIX Compatibility

`bcmp`, `bcopy`, `bzero`, `index`, `rindex`

## Safe String Functions

`strlcpy`, `strlcat`

## Case-Insensitive Comparison

`strcasecmp`, `strncasecmp`

## Bit Operations

`ffs`, `ffsl`, `ffsll`

---

# Character Classification (`ctype.h`)

```c
#include <ctype.h>
```

## Character Classification

`isalnum`, `isalpha`, `isascii`, `isblank`, `iscntrl`, `isdigit`, `isgraph`, `islower`, `isprint`, `ispunct`, `isspace`, `isupper`, `isxdigit`

## Character Conversion

`tolower`, `toupper`, `toascii`

The implementation operates on the standard ASCII character set and is suitable for freestanding environments and kernel development.

---

# Requirements

* GCC 16.1 or newer
* CMake 3.16 or newer
* GNU Make

---

# Building

## Clone Repository

### SSH

```bash
git clone --recurse-submodules git@github.com:GNUDimarik/osdev-libstdc.git
```

### HTTPS

```bash
git clone --recurse-submodules https://github.com/GNUDimarik/osdev-libstdc.git
```

## Build

```bash
cd osdev-libstdc

mkdir build && cd build

cmake -DENABLE_TEST=1 -DENABLE_BENCHMARK=1 -DARCH=x86 ../

make -j $(nproc)
```

## Running Tests

```bash
ctest --output-on-failure
```

---

# Intended Use Cases

* Operating system kernels
* Bootloaders
* Embedded systems
* Educational operating systems
* Freestanding C/C++ runtimes
* Hypervisors
* Bare-metal applications

---

# License

MIT License

Copyright (c) 2026 Dmitry Adzhiev
