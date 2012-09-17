noinst_LTLIBRARIES = liboscapcommon.la

liboscapcommon_la_SOURCES = oscapxml.c alloc.c alloc.h text.c tsort.c tsort.h \
			    util.h util.c list.h list.c bfind.c bfind.h elements.h elements.c debug.c \
			    error.c _error.h assume.h text_priv.h reporter_priv.h reporter.c debug_priv.h \
			    memusage.c memusage.h reference_priv.h reference.c

liboscapcommon_la_CPPFLAGS  = @xml2_CFLAGS@ @xslt_CFLAGS@ @exslt_CFLAGS@ -I$(srcdir)/public -I$(top_srcdir)/src/OVAL/probes/SEAP/public -I$(top_srcdir)/src
liboscapcommon_la_LIBADD = @xml2_LIBS@ @xslt_LIBS@ @exslt_LIBS@ @pthread_LIBS@

pkginclude_HEADERS =\
	public/adt.h \
	public/debug.h \
	public/error.h \
	public/htable.h \
	public/oscap.h \
	public/reference.h \
	public/reporter.h \
	public/text.h
