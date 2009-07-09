noinst_LTLIBRARIES = libxccdf.la
libxccdf_la_SOURCES = \
	benchmark.c \
	elements.c \
	item.c \
	list.c \
	profile.c \
	value.c \
	rule.c \
	item.h \
	list.h \
	elements.h
	

libxccdf_la_CFLAGS  = @libxml_cflags@
libxccdf_la_LDFLAGS = @libxml_libs@

include_HEADERS  = xccdf.h

includedir = $(prefix)/include/lib@PACKAGE@

