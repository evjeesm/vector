#!/bin/sh


autoreconf -fvi || exit 1;

./patch-test-driver.sh build-aux/test-driver build-aux/check/__inst
