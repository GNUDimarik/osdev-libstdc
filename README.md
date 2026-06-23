# osdev-libstdc
Minimal libstdc library for bare metal osdev

## Build:

#### SSH
git clone --recurse-submodules git@github.com:GNUDimarik/osdev-libstdc.git
#### HTTP
git clone --recurse-submodules https://github.com/GNUDimarik/osdev-libstdc.git

cd osdev-libstdc
mkdir build && cd build

cmake -DENABLE_TEST=1 ../
make -j $(nproc)

## Run
./osdev_libstdc_test

Work in prgress ...
