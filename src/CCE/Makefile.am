noinst_LTLIBRARIES = libcce.la

libcce_la_SOURCES  = cce.c
libcce_la_SOURCES += cce_priv.c list_refs.c list_cstring.c

libcce_la_CFLAGS   = @libxml_cflags@
libcce_la_LDFLAGS  = @libxml_libs@

include_HEADERS = cce.h
includedir = $(prefix)/include/lib@PACKAGE@
