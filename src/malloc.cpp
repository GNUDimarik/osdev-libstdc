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

#include "../include/dux/spin_lock.h"
#include "../malloc/memory.h"

namespace
{
static dux::spin_lock gLock;
}

__MAYBE_BEGIN_STD_NAMESPACE
__BEGIN_DECLS

int libstdc_allocator_initialize(void* base, size_t size)
{
    __STD_NAMESPACE::lock_guard g(gLock);
   return mem_initialize(base, size);
}

void *aligned_alloc(size_t alignment, size_t size)
{
    __STD_NAMESPACE::lock_guard g(gLock);
    return mem_malloc_aligned(alignment, size);
}

void *calloc(size_t num, size_t size)
{
    __STD_NAMESPACE::lock_guard g(gLock);
    return mem_calloc(num, size);
}

void free(void *ptr)
{
    __STD_NAMESPACE::lock_guard g(gLock);
    mem_free(ptr);
}

void *malloc(size_t size)
{
    __STD_NAMESPACE::lock_guard g(gLock);
    return mem_malloc(size);
}

void *realloc(void *ptr, size_t new_size)
{
    __STD_NAMESPACE::lock_guard g(gLock);
    return mem_realloc(ptr, new_size);
}

__END_DECLS
__MAYBE_END_STD_NAMESPACE