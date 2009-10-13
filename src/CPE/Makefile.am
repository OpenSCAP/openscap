noinst_LTLIBRARIES = libcpe.la

libcpe_la_SOURCES = cpedict.c cpelang.c cpeuri.c cpelang_priv.c
libcpe_la_CFLAGS  = @libxml_cflags@ @pcre_cflags@
libcpe_la_LDFLAGS = @libxml_libs@ @pcre_libs@

pkginclude_HEADERS = cpe.h cpedict.h cpelang.h cpeuri.h cpelang_priv.h
