noinst_LTLIBRARIES = libxccdf.la
libxccdf_la_SOURCES = \
	benchmark.c \
	elements.c \
	item.c \
	profile.c \
	value.c \
	rule.c \
	item.h \
	elements.h \
	xccdf_extensions.c \
	xccdf_policy.c
	

libxccdf_la_CFLAGS  = @libxml_cflags@ -I$(top_srcdir)/src/common/public -I$(top_srcdir)/src/XCCDF/public
libxccdf_la_LDFLAGS = @libxml_libs@

pkginclude_HEADERS  = public/xccdf.h public/xccdf_extensions.h public/xccdf_policy.h

