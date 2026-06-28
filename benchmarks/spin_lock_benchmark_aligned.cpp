#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <thread>

#include "spin_lock.h"

namespace
{

static int max_threads()
{
    const auto n = std::thread::hardware_concurrency();
    return n == 0 ? 8 : static_cast<int>(n);
}

struct padded_counter
{
#ifndef DISABLE_SPINLOCK_ALIGNMENT
    alignas(std::hardware_destructive_interference_size)
#endif
    std::uint64_t value = 0;
};

dux::spin_lock g_lock;
std::uint64_t g_shared_val = 0;

dux::spin_lock g_locks[64];
padded_counter g_counters[64];

static void BM_spin_lock_lock_unlock(benchmark::State &state)
{
    for (auto _ : state) {
        g_lock.lock();
        benchmark::ClobberMemory();
        g_lock.unlock();
    }
}

BENCHMARK(BM_spin_lock_lock_unlock)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

static void BM_spin_lock_increment(benchmark::State &state)
{
    for (auto _ : state) {
        g_lock.lock();

        benchmark::DoNotOptimize(g_shared_val);
        ++g_shared_val;
        benchmark::DoNotOptimize(g_shared_val);

        g_lock.unlock();
    }
}

BENCHMARK(BM_spin_lock_increment)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

static void BM_spin_lock_try_lock(benchmark::State &state)
{
    std::uint64_t acquired = 0;
    std::uint64_t failed = 0;

    for (auto _ : state) {
        if (g_lock.try_lock()) {
            ++acquired;
            benchmark::DoNotOptimize(acquired);
            g_lock.unlock();
        } else {
            ++failed;
            benchmark::DoNotOptimize(failed);
        }
    }

    state.counters["acquired"] = static_cast<double>(acquired);
    state.counters["failed"] = static_cast<double>(failed);
}

BENCHMARK(BM_spin_lock_try_lock)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

static void BM_spin_lock_independent_lock_unlock(benchmark::State &state)
{
    const auto index = static_cast<std::size_t>(state.thread_index());
    auto &lock = g_locks[index];

    for (auto _ : state) {
        lock.lock();
        benchmark::ClobberMemory();
        lock.unlock();
    }
}

BENCHMARK(BM_spin_lock_independent_lock_unlock)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

static void BM_spin_lock_independent_increment(benchmark::State &state)
{
    const auto index = static_cast<std::size_t>(state.thread_index());
    auto &lock = g_locks[index];
    auto &counter = g_counters[index].value;

    for (auto _ : state) {
        lock.lock();

        benchmark::DoNotOptimize(counter);
        ++counter;
        benchmark::DoNotOptimize(counter);

        lock.unlock();
    }
}

BENCHMARK(BM_spin_lock_independent_increment)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

} // namespace