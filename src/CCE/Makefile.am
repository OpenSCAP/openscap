noinst_LTLIBRARIES = libcce.la

libcce_la_SOURCES  = cce.c
libcce_la_SOURCES += cce_priv.c
libcce_la_SOURCES += cce_priv.h

libcce_la_CPPFLAGS   = @xml2_CFLAGS@ -I$(srcdir)/public -I$(top_srcdir)/src/common/public -I$(top_srcdir)/src/ -I$(top_srcdir)/src/source/public
libcce_la_LDFLAGS  = @xml2_LIBS@

pkginclude_HEADERS = public/cce.h
