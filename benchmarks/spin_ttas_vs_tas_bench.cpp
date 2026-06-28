#include <benchmark/benchmark.h>

#include "../include/atomic.h"
#include <asm/cpu.h>
#include <cstdint>
#include <thread>

namespace
{

class tas_spin_lock
{
public:
    static constexpr int kFree = 0;
    static constexpr int kLocked = 1;

    bool try_lock()
    {
        return _M_value.exchange(kLocked, __STD_NAMESPACE::memory_order_acquire) == kFree;
    }

    void lock()
    {
        while (_M_value.exchange(kLocked, __STD_NAMESPACE::memory_order_acquire) != kFree) {
            cpu_relax();
        }
    }

    void unlock()
    {
        _M_value.store(kFree, __STD_NAMESPACE::memory_order_release);
    }

private:
    alignas(__STD_NAMESPACE::hardware_destructive_interference_size)
    __STD_NAMESPACE::atomic<int> _M_value = kFree;
};

class ttas_spin_lock
{
public:
    static constexpr int kFree = 0;
    static constexpr int kLocked = 1;

    bool try_lock()
    {
        return _M_value.exchange(kLocked, __STD_NAMESPACE::memory_order_acquire) == kFree;
    }

    void lock()
    {
        for (;;) {
            while (_M_value.load(__STD_NAMESPACE::memory_order_relaxed) == kLocked) {
                cpu_relax();
            }

            if (_M_value.exchange(kLocked, __STD_NAMESPACE::memory_order_acquire) == kFree) {
                return;
            }
        }
    }

    void unlock()
    {
        _M_value.store(kFree, __STD_NAMESPACE::memory_order_release);
    }

private:
    alignas(__STD_NAMESPACE::hardware_destructive_interference_size)
    __STD_NAMESPACE::atomic<int> _M_value = kFree;
};

template <typename Lock>
struct bench_data
{
    Lock lock;
    std::uint64_t shared_val = 0;
};

bench_data<tas_spin_lock> g_tas_data;
bench_data<ttas_spin_lock> g_ttas_data;

template <typename Lock>
static void BM_lock_unlock(benchmark::State &state, bench_data<Lock> &data)
{
    for (auto _ : state) {
        data.lock.lock();
        benchmark::ClobberMemory();
        data.lock.unlock();
    }
}

template <typename Lock>
static void BM_increment(benchmark::State &state, bench_data<Lock> &data)
{
    for (auto _ : state) {
        data.lock.lock();

        benchmark::DoNotOptimize(data.shared_val);
        ++data.shared_val;
        benchmark::DoNotOptimize(data.shared_val);

        data.lock.unlock();
    }
}

static void BM_TAS_lock_unlock(benchmark::State &state)
{
    BM_lock_unlock(state, g_tas_data);
}

BENCHMARK(BM_TAS_lock_unlock)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

static void BM_TTAS_lock_unlock(benchmark::State &state)
{
    BM_lock_unlock(state, g_ttas_data);
}

BENCHMARK(BM_TTAS_lock_unlock)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

static void BM_TAS_increment(benchmark::State &state)
{
    BM_increment(state, g_tas_data);
}

BENCHMARK(BM_TAS_increment)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

static void BM_TTAS_increment(benchmark::State &state)
{
    BM_increment(state, g_ttas_data);
}

BENCHMARK(BM_TTAS_increment)
->Threads(1)
->Threads(2)
->Threads(4)
->Threads(8)
->Threads(16)
->UseRealTime();

} // namespace