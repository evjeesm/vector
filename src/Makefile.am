noinst_LTLIBRARIES = libvector_funcs.la
lib_LTLIBRARIES = libvector.la libvector_static.la

bin_PROGRAMS = polimorph
polimorph_SOURCES = polimorph.c
polimorph_LDADD = libvector_static.la

libvector_funcs_la_SOURCES = vector.c vector.h memswap.c memswap.h

libvector_la_SOURCES = 
libvector_la_LDFLAGS = -shared
libvector_la_LIBADD = libvector_funcs.la

libvector_static_la_SOURCES = 
libvector_static_la_LDFLAGS= -static
libvector_static_la_LIBADD = libvector_funcs.la

