cmake_minimum_required(VERSION 3.23)

project(osdev-libstdc LANGUAGES C CXX)

set(ARCH "x86" CACHE STRING "Target architecture")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ENABLE_TEST ON)

add_library(osdev-libstdc STATIC
        src/cstring.cpp
        src/strerror.cpp
        src/vsnprintf.cpp
        src/cctype.cpp
        malloc/memory.cpp
        malloc/memory.h
        malloc/logging.h
        posix/posix_strings.cpp
        include/libstdc/config.h
        include/libstdc/utils.h
        include/libstdc/cctype.h
        include/libstdc/printf.h
        include/posix/posix_strings.h
        include/arch/x86/asm/cpu.h
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
    message("OSDEV_FREESTANDING")
    target_compile_definitions(osdev-libstdc
            PUBLIC
            __OSDEV_FREESTANDING=1
            __OSDEV_HAVE_ERRNO_H__=1
            __OSDEV_HAVE_STRING_H__=1
            __OSDEV_HAVE_CONFIG_H__=1
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
else()
    target_compile_definitions(osdev-libstdc
            PUBLIC
            __STD_LIBC_TEST=1
    )
endif()

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
    )

    target_link_libraries(osdev_libstdc_test
            PRIVATE
            osdev-libstdc
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(osdev_libstdc_test)
endif()