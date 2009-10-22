noinst_LTLIBRARIES = libcpe.la

libcpe_la_SOURCES = cpedict.c cpelang.c cpeuri.c cpelang_priv.c cpedict_priv.c \
		    cpelang_priv.h cpedict_priv.h
libcpe_la_CPPFLAGS  = @libxml_cflags@ @pcre_cflags@ -Ipublic -I$(top_srcdir)/src/common/public
libcpe_la_LDFLAGS = @libxml_libs@ @pcre_libs@

pkginclude_HEADERS = public/cpe.h public/cpedict.h public/cpelang.h public/cpeuri.h
