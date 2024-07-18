#!/bin/sh

# we need to rebuild check framework in mingw:
git clone https://github.com/libcheck/check.git build-aux/check
cd build-aux/check
autoreconf -fvi || exit 1;

./configure CC=x86_64-w64-mingw32-gcc CFLAGS='-Wall -Wextra' --enable-subunit=no --host=x86_64-w64-mingw32 --prefix=$(pwd)/__inst
make
make install

# patch test-driver to use wine for checking check (LOL)
../../patch-test-driver.sh $(pwd)/test-driver $(pwd)/__inst
make check

# generating config files for vector project
cd ../../; autoreconf -fvi || exit 1;

# configure vector with wine-check
./configure CFLAGS='-Wall -Wextra -Wno-override-init' \
    CHECK_FLAGS="-I$(pwd)/build-aux/check/__inst/include/" \
    CHECK_LIBS="$(pwd)/build-aux/check/__inst/bin/libcheck-0.dll" \
    --host x86_64-w64-mingw32

# patch test driver for the vector
./patch-test-driver.sh $(pwd)/build-aux/test-driver $(pwd)/build-aux/check/__inst
