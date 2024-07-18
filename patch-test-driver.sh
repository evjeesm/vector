#!/bin/sh

if [ $# -eq 2 ]; then
    WINE_PREFIX="WINEPATH=\'$2/bin/\;/usr/x86_64-w64-mingw32/lib\' wine "
    PATTERN="s,\(\"\$@\"\),$WINE_PREFIX\1,g"
    sed -i "$PATTERN" $1
else
    echo "Provide test-driver path and check install path!"
fi


