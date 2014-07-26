noinst_LTLIBRARIES = liboscapsource.la

liboscapsource_la_SOURCES = \
	doc_type.c \
	doc_type_priv.h \
	oscap_source.c \
	oscap_source_priv.h

liboscapsource_la_CPPFLAGS  = \
	@curl_CFLAGS@ \
	@xml2_CFLAGS@ @xslt_CFLAGS@ @exslt_CFLAGS@ \
	-I$(srcdir)/public -I$(top_srcdir)/src/OVAL/probes/SEAP/public -I$(top_srcdir)/src

liboscapsource_la_LIBADD = \
	@curl_LIBS@ \
	@xml2_LIBS@ @xslt_LIBS@ @exslt_LIBS@ @pthread_LIBS@
