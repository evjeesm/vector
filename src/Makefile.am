## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libvector.la libvector_mock.la
libvector_la_SOURCES = vector.c vector.h
libvector_la_LDFLAGS = -lm

libvector_mock_la_SOURCES = vector.c vector.h
libvector_mock_la_CFLAGS = -DMOCK_MALLOC
libvector_mock_la_LDFLAGS = -lm


# bin_PROGRAMS = main
# main_SOURCES = main.c
# main_LDADD = libvector.la
