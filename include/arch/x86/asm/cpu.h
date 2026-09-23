/**
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

#ifndef ARCH_X86_ASM_CPU_H_
#define ARCH_X86_ASM_CPU_H_

#include <stdint.h>

namespace x86::cpu {
static constexpr uint32_t EFLAGS_IF = 1u << 9;
}

static inline void cpu_relax() noexcept
{
    __asm__ __volatile__("pause" ::: "memory");
}

static inline void cpu_halt() noexcept
{
    __asm__ __volatile__("hlt" ::: "memory");
}

static inline void irq_disable() noexcept
{
    __asm__ __volatile__("cli" ::: "memory");
}

static inline void irq_enable() noexcept
{
  __asm__ __volatile__("sti" ::: "memory");
}

[[nodiscard]]
inline bool irq_save_disable() noexcept {
    uint32_t flags;

    __asm__ __volatile__("pushfl\n\t"
                         "popl %[flags]\n\t"
                         "cli"
                         : [flags] "=r"(flags)
                         :
                         : "memory");

    return (flags & x86::cpu::EFLAGS_IF) != 0;
}

inline void irq_restore(bool was_enabled) noexcept {
    if (was_enabled) {
        irq_enable();
    }
}

#endif // #endif