
ACLOCAL_AMFLAGS = -I m4 -I/usr/share/aclocal
SUBDIRS = src . tests


include $(top_srcdir)/aminclude_static.am
clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean


debug-vector-test: ./src/libvector.la ./tests/vector_test
	LD_LIBRARY_PATH=./src/.libs:/usr/local/lib CK_FORK=no gdb -tui ./tests/.libs/vector_test

debug-vector-test-failures: ./src/libvector.la ./tests/vector_test_failures
	LD_LIBRARY_PATH=./src/.libs:/usr/local/lib CK_FORK=no gdb -tui ./tests/.libs/vector_test_failures
