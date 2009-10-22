noinst_LTLIBRARIES = libcce.la

libcce_la_SOURCES  = cce.c
libcce_la_SOURCES += cce_priv.c
libcce_la_SOURCES += cce_priv.h

libcce_la_CPPFLAGS   = @libxml_cflags@ -Ipublic -I$(top_srcdir)/src/common/public
libcce_la_LDFLAGS  = @libxml_libs@

pkginclude_HEADERS = public/cce.h
