ACLOCAL_AMFLAGS = -I m4 -I/usr/share/aclocal
SUBDIRS = src . tests

debug-vector-test: ./src/libvector.la ./tests/vector_test
	LD_LIBRARY_PATH=./src/.libs:/usr/local/lib CK_FORK=no gdb -tui ./tests/.libs/vector_test

debug-vector-test-other: ./src/libvector.la ./tests/vector_test_other
	LD_LIBRARY_PATH=./src/.libs:/usr/local/lib CK_FORK=no gdb -tui ./tests/.libs/vector_test_other
