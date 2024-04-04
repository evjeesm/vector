
VALGRIND_memcheck_FLAGS = --leak-check=full --track-origins=yes
@VALGRIND_CHECK_RULES@

TESTS = vector_test vector_test_failures
check_PROGRAMS = vector_test vector_test_failures

vector_test_SOURCES = vector_test.c $(top_builddir)/src/vector.h
vector_test_CFLAGS = @CHECK_CFLAGS@
vector_test_LDADD = $(top_builddir)/src/libvector.la @CHECK_LIBS@

vector_test_failures_SOURCES = vector_test_failures.c $(top_builddir)/src/vector.h
vector_test_failures_CFLAGS = @CHECK_CFLAGS@
vector_test_failures_LDADD = $(top_builddir)/src/libvector.la @CHECK_LIBS@

# VALGRIND_SUPPRESSIONS_FILES = my-project.supp
# EXTRA_DIST = my-project.supp
