cmake_minimum_required(VERSION 3.23)

project(osdev-libstdc LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ENABLE_TEST ON)
# hardcoded for now
set(ARCH x86)

add_library(osdev-libstdc STATIC
        src/cstring.cpp
        src/strerror.cpp
        src/vsnprintf.cpp
        src/cctype.cpp
        malloc/memory.cpp
        src/malloc.cpp
        malloc/memory.h
        malloc/logging.h
        posix/posix_strings.cpp
        include/libstdc/config.h
        include/libstdc/utils.h
        include/libstdc/cctype.h
        include/libstdc/printf.h
        include/posix/posix_strings.h
        include/arch/x86/asm/cpu.h
        include/arch/x86/asm/barrier.h
        include/dux/spin_lock.h
        include/malloc.h
        include/atomic.h
        src/abort.cpp
        include/abort.h
)

target_include_directories(osdev-libstdc
        PUBLIC
        "${PROJECT_SOURCE_DIR}/include/libstdc"
        "${PROJECT_SOURCE_DIR}/include/posix"
        "${PROJECT_SOURCE_DIR}/include/arch/${ARCH}"
)

target_compile_options(osdev-libstdc
        PRIVATE
        -Wall
        -fno-exceptions
        -fno-rtti
        -fno-stack-protector
        -nostdlib
        -std=gnu++20
)

if (DEFINED OSDEV_FREESTANDING)
    target_include_directories(osdev-libstdc
            PUBLIC
            "${PROJECT_SOURCE_DIR}/include/"
            "${PROJECT_SOURCE_DIR}/include/libstdc"
            "${PROJECT_SOURCE_DIR}/include/posix"
            "${PROJECT_SOURCE_DIR}/include/arch/${ARCH}")

    target_compile_definitions(osdev-libstdc
            PUBLIC
            __OSDEV_FREESTANDING=1
            __OSDEV_HAVE_ERRNO_H__=1
            __OSDEV_HAVE_STRING_H__=1
            __OSDEV_HAVE_CONFIG_H__=1
            __OSDEV_DUX_LIBSTDC__
            __POSIX_STRINGS_MACRO__
    )

    target_compile_options(osdev-libstdc
            PRIVATE
            -Wall
            -ffreestanding
            -fno-exceptions
            -fno-rtti
            -fno-stack-protector
            -nostdlib
            -std=gnu++20
    )

    target_link_options(osdev-libstdc
            PRIVATE
            -nostdlib
            -nodefaultlibs
    )
else ()
    target_compile_definitions(osdev-libstdc
            PUBLIC
            __STD_LIBC_TEST=1
    )
endif ()

if (DEFINED ENABLE_TEST AND NOT DEFINED OSDEV_FREESTANDING)
    cmake_policy(SET CMP0135 NEW)

    include(FetchContent)

    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(osdev_libstdc_test
            tests/cstring_test.cpp
            tests/vsnprintf_test.cpp
            tests/ctype_test.cpp
            malloc/allocator_test.cpp
            tests/spin_lock_test.cpp
            tests/atomic_test.cpp
    )

    target_link_libraries(osdev_libstdc_test
            PRIVATE
            osdev-libstdc
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(osdev_libstdc_test)
endif ()