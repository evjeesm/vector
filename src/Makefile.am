noinst_LTLIBRARIES = libvector_funcs.la
libvector_funcs_la_SOURCES = vector.c vector.h memswap.c memswap.h

if !MINGW

lib_LTLIBRARIES = libvector.la libvector_static.la
libvector_la_SOURCES = 
libvector_la_LDFLAGS = -shared
libvector_la_LIBADD = libvector_funcs.la
libvector_la_LIBS = $(CODE_COVERAGE_LIBS)
libvector_la_CPPFLAGS = $(CODE_COVERAGE_CPPFLAGS)
libvector_la_CFLAGS = $(CODE_COVERAGE_CFLAGS)
libvector_la_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

else
lib_LTLIBRARIES = libvector_static.la
endif

libvector_static_la_SOURCES = 
libvector_static_la_LDFLAGS= -static
libvector_static_la_LIBADD = libvector_funcs.la
libvector_static_la_LIBS = $(CODE_COVERAGE_LIBS)
libvector_static_la_CPPFLAGS = $(CODE_COVERAGE_CPPFLAGS)
libvector_static_la_CFLAGS = $(CODE_COVERAGE_CFLAGS)
libvector_static_la_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

bin_PROGRAMS = polimorph
polimorph_SOURCES = polimorph.c
polimorph_LDADD = libvector_static.la
polimorph_LIBS = $(CODE_COVERAGE_LIBS)
polimorph_CPPFLAGS = $(CODE_COVERAGE_CPPFLAGS)
polimorph_CFLAGS = $(CODE_COVERAGE_CFLAGS)
polimorph_CXXFLAGS = $(CODE_COVERAGE_CXXFLAGS)

include_HEADERS = vector.h

