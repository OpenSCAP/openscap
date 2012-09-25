
noinst_LTLIBRARIES = libxccdf.la

libxccdf_la_SOURCES = \
	benchmark.c \
	elements.c \
	item.c \
	profile.c \
	value.c \
	resolve.c \
	result.c \
	rule.c \
	item.h \
	elements.h \
	helpers.h \
	unused.h \
	xccdf_impl.h


libxccdf_la_CPPFLAGS  = @xml2_CFLAGS@ \
			-I$(top_srcdir)/src \
			-I$(top_srcdir)/src/XCCDF/public \
			-I$(top_srcdir)/src/common/public \
			-I$(top_srcdir)/lib

libxccdf_la_LDFLAGS = @xml2_LIBS@

pkginclude_HEADERS  = public/xccdf_benchmark.h

