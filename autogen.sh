#!/bin/sh

# we need to rebuild check framework in mingw:
git clone https://github.com/libcheck/check.git build-aux/check
cd build-aux/check
autoreconf -i || exit 1;

./configure CFLAGS='-Wall -Wextra' --enable-subunit=no --host x86_64-w64-mingw32 --prefix=$(pwd)/__inst
make
make check
make install

echo "Leaving $(pwd)"; cd -;

autoreconf -fvi || exit 1;

./configure CFLAGS='-Wall -Wextra -O3 -Wno-override-init' CHECK_FLAGS="-I$(pwd)/build-aux/check/__inst/include/" CHECK_LIBS="$(pwd)/build-aux/check/__inst/bin/libcheck-0.dll" --host x86_64-w64-mingw32

./patch-test-driver.sh
