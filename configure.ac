#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_CONFIG_MACRO_DIRS([m4])
AC_INIT([Vector], [1.0], [https://github.com/evjeesm])
AC_CONFIG_AUX_DIR([build-aux])

# valgrind
AX_VALGRIND_DFLT([memcheck], [on])
AX_VALGRIND_DFLT([helgrind], [off])
AX_VALGRIND_DFLT([sgcheck], [off])
AX_VALGRIND_DFLT([drd], [off])
AX_VALGRIND_CHECK

# unique source file -- primitive safety check
AC_CONFIG_SRCDIR([src/vector.c])

AM_INIT_AUTOMAKE([-Wall -Wportability foreign 1.11.2])
AM_CONDITIONAL(MINGW, [test "$MSYSTEM_CHOST" = x86_64-w64-mingw32 || "$host" = x86_64-w64-mingw32])

# Checks for programs.
AM_PROG_AR
LT_INIT
AC_PROG_CC
AC_PROG_CPP
AC_PROG_AWK

# Checks for libraries.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])

# Checks for header files.
AC_CHECK_HEADERS([stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_RESTRICT
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove memset])

# Output files 
AC_CONFIG_HEADERS([config.h])

AX_CODE_COVERAGE

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 tests/Makefile
                 examples/Makefile])
AC_OUTPUT
