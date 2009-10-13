noinst_LTLIBRARIES = libcpe.la

libcpe_la_SOURCES = cpedict.c cpelang.c cpeuri.c cpelang_priv.c cpedict_priv.c \
		    cpelang_priv.h cpedict_priv.h
libcpe_la_CFLAGS  = @libxml_cflags@ @pcre_cflags@
libcpe_la_LDFLAGS = @libxml_libs@ @pcre_libs@

pkginclude_HEADERS = cpe.h cpedict.h cpelang.h cpeuri.h
