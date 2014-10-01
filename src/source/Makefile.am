noinst_LTLIBRARIES = liboscapsource.la

liboscapsource_la_SOURCES = \
	doc_type.c \
	doc_type_priv.h \
	oscap_source.c \
	oscap_source_priv.h \
	schematron.c \
	schematron_priv.h \
	validate.c \
	validate_priv.h \
	xslt.c \
	xslt_priv.h

liboscapsource_la_CPPFLAGS  = \
	@curl_CFLAGS@ \
	@xml2_CFLAGS@ @xslt_CFLAGS@ @exslt_CFLAGS@ \
	-I$(srcdir)/public \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/CPE/public \
	-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
	-I$(top_srcdir)/src/common/public

liboscapsource_la_LIBADD = \
	@curl_LIBS@ \
	@xml2_LIBS@ @xslt_LIBS@ @exslt_LIBS@ @pthread_LIBS@

pkginclude_HEADERS =\
	public/oscap_source.h
