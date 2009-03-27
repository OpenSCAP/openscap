noinst_LTLIBRARIES = libcpe.la

libcpe_la_SOURCES = cpedict.c  cpelang.c  cpematch.c cpeuri.c
libcpe_la_CFLAGS  = @libxml_cflags@ @pcre_cflags@
libcpe_la_LDFLAGS = @libxml_libs@ @pcre_libs@

include_HEADERS = cpe.h cpedict.h  cpelang.h  cpematch.h cpeuri.h
includedir = $(prefix)/include/lib@PACKAGE@
