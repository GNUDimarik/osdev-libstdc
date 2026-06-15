set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR i686)

set(CMAKE_C_COMPILER /mnt/d/osdev/i686-elf/bin/i686-elf-gcc)
set(CMAKE_CXX_COMPILER /mnt/d/osdev/i686-elf/bin/i686-elf-g++)

set(CMAKE_AR /mnt/d/osdev/i686-elf/bin/i686-elf-ar)
set(CMAKE_RANLIB /mnt/d/osdev/i686-elf/bin/i686-elf-ranlib)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_FLAGS_INIT "-Wall -ffreestanding -fno-stack-protector -nostdlib")
set(CMAKE_CXX_FLAGS_INIT "-Wall -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -nostdlib -std=gnu++20")