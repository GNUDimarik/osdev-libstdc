include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.1
)

FetchContent_MakeAvailable(googlebenchmark)

add_executable(spin_lock_aligned_bench
        spin_lock_benchmark_aligned.cpp
)

target_link_libraries(spin_lock_aligned_bench
        PRIVATE
        benchmark::benchmark_main
)

target_include_directories(spin_lock_aligned_bench
        PRIVATE
        "../include/arch/${ARCH}"
        "../include/dux"
        "../include/libstdc")

target_compile_definitions(spin_lock_aligned_bench
        PUBLIC
        __STD_LIBC_TEST=1
)

add_executable(spin_lock_unaligned_bench
        spin_lock_benchmark_unaligned.cpp
)

target_link_libraries(spin_lock_unaligned_bench
        PRIVATE
        benchmark::benchmark_main
)

target_include_directories(spin_lock_unaligned_bench
        PRIVATE
        "../include/arch/${ARCH}"
        "../include/dux"
        "../include/libstdc"
)

target_compile_definitions(spin_lock_unaligned_bench
        PRIVATE
        DISABLE_SPINLOCK_ALIGNMENT
        __STD_LIBC_TEST=1
)

add_executable(spin_ttas_vs_tas_bench
        spin_ttas_vs_tas_bench.cpp
)

target_link_libraries(spin_ttas_vs_tas_bench
        PRIVATE
        benchmark::benchmark_main
)

target_include_directories(spin_ttas_vs_tas_bench
        PRIVATE
        "../include/arch/${ARCH}"
        "../include/dux"
        "../include/libstdc"
)

target_compile_definitions(spin_ttas_vs_tas_bench
        PRIVATE
        DISABLE_SPINLOCK_ALIGNMENT
        __STD_LIBC_TEST=1
)