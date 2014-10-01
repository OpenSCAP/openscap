
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
	tailoring.c \
	item.h \
	elements.h \
	helpers.h \
	unused.h \
	xccdf_impl.h \
	xccdf_session.c

libxccdf_la_CPPFLAGS  = @xml2_CFLAGS@ \
			-I$(top_srcdir)/src \
			-I$(top_srcdir)/src/XCCDF/public \
			-I$(top_srcdir)/src/XCCDF_POLICY/public \
			-I$(top_srcdir)/src/SCE/public \
			-I$(top_srcdir)/src/common/public \
			-I$(top_srcdir)/src/source/public \
			-I$(top_srcdir)/src/OVAL/public \
			-I$(top_srcdir)/src/CPE/public \
			-I$(top_srcdir)/lib

libxccdf_la_LDFLAGS = @xml2_LIBS@

pkginclude_HEADERS  =\
	public/xccdf_benchmark.h \
	public/xccdf_session.h

