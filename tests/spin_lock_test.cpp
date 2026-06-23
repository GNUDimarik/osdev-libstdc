#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include "../dux/spin_lock.h"

TEST(SpinLockTest, MutualExclusionGemini)
{
    dux::spin_lock spinLock;
    int shared_counter = 0;
    const int num_threads = 10;
    const int num_iterations = 100000;

    auto worker = [&]()
    {
        for (int i = 0; i < num_iterations; ++i) {
            __STD_NAMESPACE::lock_guard guard(spinLock);
            shared_counter++; // Critical section
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    // Spawn threads
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker);
    }

    // Wait for all threads to finish
    for (auto &t: threads) {
        t.join();
    }

    // If mutual exclusion fails, some increments will be overwritten
    EXPECT_EQ(shared_counter, num_threads * num_iterations);
}

TEST(SpinLockTest, TryLock)
{
    dux::spin_lock lock;
    lock.lock();
    EXPECT_FALSE(lock.try_lock());
    lock.unlock();
    EXPECT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(SpinLockTest, TryLockContention)
{
    dux::spin_lock lock;
    constexpr int kThreads = 8;
    std::atomic<int> success_count{0};
    std::vector<std::thread> threads;
    lock.lock();

    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back([&]()
                             {

                                 if (lock.try_lock()) {
                                     ++success_count;
                                     lock.unlock();
                                 }
                             });
    }

    for (auto &t: threads) {
        t.join();
    }

    EXPECT_EQ(success_count.load(), 0);

    lock.unlock();
}

TEST(SpinLockTest, Stress)
{
    dux::spin_lock lock;
    constexpr int kThreads = 16;
    constexpr int kIterations = 100000;
    std::uint64_t counter = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back([&, seed = i]() mutable
                             {
                                 std::uint32_t state = seed + 1;

                                 for (int j = 0; j < kIterations; ++j) {

                                     state ^= state << 13;
                                     state ^= state >> 17;
                                     state ^= state << 5;

                                     {
                                         __STD_NAMESPACE::lock_guard guard(lock);
                                         ++counter;
                                     }

                                     if ((state & 0xF) == 0) {
                                         std::this_thread::yield();
                                     }
                                 }
                             });
    }

    for (auto &t: threads) {
        t.join();
    }

    EXPECT_EQ(counter,
              static_cast<std::uint64_t>(kThreads) * kIterations);
}

TEST(SpinLockTest, MutualExclusionGPT)
{
    dux::spin_lock lock;
    constexpr int kThreads = 8;
    constexpr int kIterations = 100000;
    int inside = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back([&]()
                             {
                                 for (int j = 0; j < kIterations; ++j) {
                                     __STD_NAMESPACE::lock_guard guard(lock);
                                     ++inside;
                                     EXPECT_EQ(inside, 1);
                                     --inside;
                                 }
                             });
    }

    for (auto &t: threads) {
        t.join();
    }
}

TEST(SpinLockTest, CounterIncrement)
{
    dux::spin_lock lock;
    constexpr int kThreads = 8;
    constexpr int kIterations = 100000;
    std::uint64_t counter = 0;
    std::vector<std::thread> threads;

    for (int i = 0; i < kThreads; ++i) {
        threads.emplace_back([&]()
                             {
                                 for (int j = 0; j < kIterations; ++j) {
                                     __STD_NAMESPACE::lock_guard guard(lock);
                                     ++counter;
                                 }
                             });
    }

    for (auto &t: threads) {
        t.join();
    }

    EXPECT_EQ(counter, kThreads * kIterations);
}

TEST(SpinLockTest, LockGuardUnlocksOnScopeExit)
{
    dux::spin_lock spinLock;
    {
        __STD_NAMESPACE::lock_guard guard(spinLock);
        EXPECT_FALSE(spinLock.try_lock());
    }

    EXPECT_TRUE(spinLock.try_lock());
    spinLock.unlock();
}