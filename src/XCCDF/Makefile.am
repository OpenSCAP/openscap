noinst_LTLIBRARIES = libxccdf.la
libxccdf_la_SOURCES = \
	benchmark.c \
	elements.c \
	item.c \
	profile.c \
	value.c \
	rule.c \
	item.h \
	elements.h
	

libxccdf_la_CFLAGS  = @libxml_cflags@ -I$(top_srcdir)/src/common/public
libxccdf_la_LDFLAGS = @libxml_libs@

pkginclude_HEADERS  = public/xccdf.h

