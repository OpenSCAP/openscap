noinst_LTLIBRARIES = libxccdf.la
libxccdf_la_SOURCES = \
	benchmark.c \
	elements.c \
	item.c \
	profile.c \
	value.c \
	result.c \
	rule.c \
	item.h \
	elements.h \
	helpers.h
	

libxccdf_la_CFLAGS  = @libxml_cflags@ -I$(top_srcdir)/src/common/public -I$(top_srcdir)/src/XCCDF/public
libxccdf_la_LDFLAGS = @libxml_libs@

pkginclude_HEADERS  = public/xccdf.h

