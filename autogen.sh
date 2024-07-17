#!/bin/sh

# we need to rebuild check framework in mingw:
git clone https://github.com/libcheck/check.git build-aux/check
cd build-aux/check
autoreconf -i || exit 1;

./configure CC=x86_64-w64-mingw32-gcc CFLAGS='-Wall -Wextra' --enable-subunit=no --host=x86_64-w64-mingw32 --prefix=$(pwd)/__inst
make
make check
make install

cd ../../;

autoreconf -fvi || exit 1;

./patch-test-driver.sh
