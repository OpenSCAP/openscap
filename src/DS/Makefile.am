
noinst_LTLIBRARIES = libds.la
libds_la_SOURCES = \
	sds.c \
	rds.c

libds_la_CPPFLAGS  = @xml2_CFLAGS@ -I$(top_srcdir)/src -I$(top_srcdir)/src/DS/public -I$(top_srcdir)/src/common/public -I$(top_srcdir)/lib
libds_la_LDFLAGS = @xml2_LIBS@
libds_la_LIBADD =

pkginclude_HEADERS  = public/ds.h

