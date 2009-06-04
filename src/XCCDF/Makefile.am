noinst_LTLIBRARIES = libxccdf.la
libxccdf_la_SOURCES = \
	benchmark.c \
	elements.c \
	item.c \
	list.c \
	profile.c \
	rule.c

libxccdf_la_CFLAGS  = @libxml_cflags@
libxccdf_la_LDFLAGS = @libxml_libs@

include_HEADERS  = xccdf.h

includedir = $(prefix)/include/lib@PACKAGE@

