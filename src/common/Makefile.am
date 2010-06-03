noinst_LTLIBRARIES = liboscapcommon.la

liboscapcommon_la_SOURCES = oscap.c alloc.c text.c tsort.c tsort.h \
			    util.h util.c list.h list.c bfind.c bfind.h elements.h elements.c debug.c \
			    error.c _error.h assume.h text_priv.h reporter_priv.h reporter.c debug_priv.h

liboscapcommon_la_CPPFLAGS  = @libxml_cflags@ -I$(srcdir)/public
liboscapcommon_la_LIBADD = @libxml_libs@ $(pthread_ldflags)

pkginclude_HEADERS  = public/oscap.h public/alloc.h public/debug.h public/error.h public/text.h public/reporter.h

