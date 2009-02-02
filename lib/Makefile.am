
AM_CFLAGS = -fPIC

lib_LTLIBRARIES = libcvsscalc.la libcve.la
include_HEADERS = libcvsscalc.h libcve.h

libcvsscalc_la_SOURCES = libcvsscalc.c
libcvsscalc_la_LIBADD = -lm

libcve_la_SOURCES = libcve.c
libcve_la_CPPFLAGS = -I$(top_srcdir) $(libxml_cflags)
libcve_la_LIBADD = $(libxml_libs)

