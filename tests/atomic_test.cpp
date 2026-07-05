#include <gtest/gtest.h>

#include "../include/atomic.h"

TEST(Atomic, BasicOperations)
{
    __STD_NAMESPACE::atomic<int> a;
    __STD_NAMESPACE::atomic<int> b(10);

    a = 1;
    EXPECT_EQ(a.load(), 1);

    a.store(2);
    EXPECT_EQ(a.load(__STD_NAMESPACE::memory_order_seq_cst), 2);

    a.store(3, __STD_NAMESPACE::memory_order_release);
    EXPECT_EQ(a.load(__STD_NAMESPACE::memory_order_acquire), 3);

    EXPECT_EQ(a.exchange(4), 3);
    EXPECT_EQ(a.load(), 4);

    EXPECT_EQ(a.exchange(5, __STD_NAMESPACE::memory_order_acq_rel), 4);
    EXPECT_EQ(a.load(), 5);

    int expected = 5;
    EXPECT_TRUE(a.compare_exchange_weak(expected, 6));

    expected = 6;
    EXPECT_TRUE(a.compare_exchange_weak(expected, 7, __STD_NAMESPACE::memory_order_acq_rel));

    expected = 7;
    EXPECT_TRUE(a.compare_exchange_weak(expected, 8,
                                        __STD_NAMESPACE::memory_order_acq_rel,
                                        __STD_NAMESPACE::memory_order_acquire));

    expected = 123;
    EXPECT_FALSE(a.compare_exchange_weak(expected, 9,
                                         __STD_NAMESPACE::memory_order_release,
                                         __STD_NAMESPACE::memory_order_relaxed));
    EXPECT_EQ(expected, 8);

    expected = 8;
    EXPECT_TRUE(a.compare_exchange_strong(expected, 9));

    expected = 9;
    EXPECT_TRUE(a.compare_exchange_strong(expected, 10, __STD_NAMESPACE::memory_order_acq_rel));

    expected = 10;
    EXPECT_TRUE(a.compare_exchange_strong(expected, 11,
                                          __STD_NAMESPACE::memory_order_acq_rel,
                                          __STD_NAMESPACE::memory_order_acquire));

    int value = a;
    EXPECT_EQ(value, 11);

    EXPECT_TRUE(a.is_lock_free());
    EXPECT_EQ(b.load(), 10);
}

TEST(Atomic, VolatileOperations)
{
    volatile __STD_NAMESPACE::atomic<int> a(1);

    a = 2;
    EXPECT_EQ(a.load(), 2);

    a.store(3);
    EXPECT_EQ(a.load(__STD_NAMESPACE::memory_order_seq_cst), 3);

    a.store(4, __STD_NAMESPACE::memory_order_release);
    EXPECT_EQ(a.load(__STD_NAMESPACE::memory_order_acquire), 4);

    EXPECT_EQ(a.exchange(5), 4);
    EXPECT_EQ(a.load(), 5);

    EXPECT_EQ(a.exchange(6, __STD_NAMESPACE::memory_order_acq_rel), 5);
    EXPECT_EQ(a.load(), 6);

    int expected = 6;
    EXPECT_TRUE(a.compare_exchange_weak(expected, 7));

    expected = 7;
    EXPECT_TRUE(a.compare_exchange_weak(expected, 8, __STD_NAMESPACE::memory_order_acq_rel));

    expected = 8;
    EXPECT_TRUE(a.compare_exchange_weak(expected, 9,
                                        __STD_NAMESPACE::memory_order_acq_rel,
                                        __STD_NAMESPACE::memory_order_acquire));

    expected = 123;
    EXPECT_FALSE(a.compare_exchange_weak(expected, 10,
                                         __STD_NAMESPACE::memory_order_release,
                                         __STD_NAMESPACE::memory_order_relaxed));
    EXPECT_EQ(expected, 9);

    expected = 9;
    EXPECT_TRUE(a.compare_exchange_strong(expected, 10));

    expected = 10;
    EXPECT_TRUE(a.compare_exchange_strong(expected, 11, __STD_NAMESPACE::memory_order_acq_rel));

    expected = 11;
    EXPECT_TRUE(a.compare_exchange_strong(expected, 12,
                                          __STD_NAMESPACE::memory_order_acq_rel,
                                          __STD_NAMESPACE::memory_order_acquire));

    int value = a;
    EXPECT_EQ(value, 12);

    EXPECT_TRUE(a.is_lock_free());
}

TEST(AtomicRef, BasicOperations)
{
    int value = 0;

    __STD_NAMESPACE::atomic_ref<int> ref(value);

    EXPECT_EQ(value, 0);
    EXPECT_EQ(ref.load(), 0);

    ref.store(10);
    EXPECT_EQ(value, 10);
    EXPECT_EQ(ref.load(), 10);

    ref = 20;
    EXPECT_EQ(value, 20);
    EXPECT_EQ(ref.load(), 20);

    EXPECT_EQ(ref.exchange(30), 20);
    EXPECT_EQ(value, 30);

    int expected = 30;
    EXPECT_TRUE(ref.compare_exchange_strong(expected, 40));
    EXPECT_EQ(value, 40);
    EXPECT_EQ(expected, 30);

    expected = 30;
    EXPECT_FALSE(ref.compare_exchange_strong(expected, 50));
    EXPECT_EQ(value, 40);
    EXPECT_EQ(expected, 40);
}

TEST(AtomicRef, ModifiesOriginalObject)
{
    uint8_t byte = 0;

    __STD_NAMESPACE::atomic_ref<uint8_t> ref(byte);

    ref.store(1, __STD_NAMESPACE::memory_order_release);

    EXPECT_EQ(byte, 1);

    EXPECT_EQ(ref.load(__STD_NAMESPACE::memory_order_acquire), 1);
}