
VALGRIND_memcheck_FLAGS = --leak-check=full --track-origins=yes
@VALGRIND_CHECK_RULES@

TESTS = vector_test vector_test_failures
check_PROGRAMS = vector_test vector_test_failures

vector_test_SOURCES = vector_test.c $(top_builddir)/src/vector.h
vector_test_LDADD = $(top_builddir)/src/libvector_static.la @CHECK_LIBS@
vector_test_LIBS = $(CODE_COVERAGE_LIBS)
vector_test_CPPFLAGS = $(CODE_COVERAGE_CPPFLAGS)
vector_test_CFLAGS = @CHECK_CFLAGS@ $(CODE_COVERAGE_CFLAGS)
vector_test_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

vector_test_failures_SOURCES = vector_test_failures.c $(top_builddir)/src/vector.h
vector_test_failures_LDADD = $(top_builddir)/src/libvector_static.la @CHECK_LIBS@

vector_test_failures_LIBS = $(CODE_COVERAGE_LIBS)
vector_test_failures_CFLAGS = @CHECK_CFLAGS@ $(CODE_COVERAGE_CFLAGS)
vector_test_failures_CPPFLAGS = $(CODE_COVERAGE_CPPFLAGS)
vector_test_failures_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

debug-vector-test: $(top_builddir)/src/libvector_static.la vector_test
	LD_LIBRARY_PATH=$(top_builddir)/src:/usr/local/lib CK_FORK=no gdb -tui vector_test

debug-vector-test-failures: $(top_builddir)/src/libvector_static.la vector_test_failures
	LD_LIBRARY_PATH=$(tip_builddir)/src:/usr/local/lib CK_FORK=no gdb -tui vector_test_failures

# VALGRIND_SUPPRESSIONS_FILES = my-project.supp
# EXTRA_DIST = my-project.supp
