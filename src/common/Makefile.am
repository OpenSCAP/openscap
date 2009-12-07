noinst_LTLIBRARIES = liboscapcommon.la

liboscapcommon_la_SOURCES = oscap.c alloc.c \
			    util.h util.c list.h list.c bfind.c bfind.h elements.h elements.c debug.c
liboscapcommon_la_CPPFLAGS  = @libxml_cflags@ -Ipublic
liboscapcommon_la_LDFLAGS = @libxml_libs@

pkginclude_HEADERS  = public/oscap.h public/alloc.h public/debug.h

