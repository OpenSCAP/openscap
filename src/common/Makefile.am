noinst_LTLIBRARIES = liboscapcommon.la

liboscapcommon_la_SOURCES = \
	alloc.c alloc.h \
	assume.h \
	bfind.c bfind.h \
	debug.c debug_priv.h \
	elements.c elements.h \
	error.c _error.h \
	list.c list.h \
	memusage.c memusage.h \
	oscap_acquire.c oscap_acquire.h \
	oscapxml.c \
	reference.c reference_priv.h \
	text.c text_priv.h \
	tsort.c tsort.h \
	util.c util.h

liboscapcommon_la_CPPFLAGS  = \
	@curl_CFLAGS@ \
	@xml2_CFLAGS@ @xslt_CFLAGS@ @exslt_CFLAGS@ \
	-I$(srcdir)/public -I$(top_srcdir)/src/OVAL/probes/SEAP/public -I$(top_srcdir)/src

liboscapcommon_la_LIBADD = \
	@curl_LIBS@ \
	@xml2_LIBS@ @xslt_LIBS@ @exslt_LIBS@ @pthread_LIBS@

pkginclude_HEADERS =\
	public/oscap_error.h \
	public/oscap.h \
	public/oscap_reference.h \
	public/oscap_text.h
