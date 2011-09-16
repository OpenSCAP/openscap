noinst_LTLIBRARIES = libcpe.la

libcpe_la_SOURCES = cpedict.c cpelang.c cpeuri.c cpelang_priv.c cpedict_priv.c \
		    cpelang_priv.h cpedict_priv.h
libcpe_la_CPPFLAGS  = @xml2_CFLAGS@ @pcre_CFLAGS@ -I$(srcdir)/public -I$(top_srcdir)/src/common/public -I$(top_srcdir)/src
libcpe_la_LDFLAGS = @xml2_LIBS@ @pcre_LIBS@

pkginclude_HEADERS = public/cpe.h public/cpedict.h public/cpelang.h public/cpeuri.h
