#!/bin/sh

WINE_PREFIX="WINEPATH=$(pwd)/check/__inst/bin/\;/usr/x86_64-w64-mingw32/lib wine "
PATTERN="s,\(\"\$@\"\),$WINE_PREFIX\1,g"

sed -i "$PATTERN" build-aux/test-driver

