noinst_LTLIBRARIES += liboscapsource.la

liboscapsource_la_SOURCES = \
	%reldir%/bz2.c \
	%reldir%/bz2_priv.h \
	%reldir%/doc_type.c \
	%reldir%/doc_type_priv.h \
	%reldir%/oscap_source.c \
	%reldir%/oscap_source_priv.h \
	%reldir%/schematron.c \
	%reldir%/schematron_priv.h \
	%reldir%/validate.c \
	%reldir%/validate_priv.h \
	%reldir%/xslt.c \
	%reldir%/xslt_priv.h

liboscapsource_la_CPPFLAGS  = \
	@curl_CFLAGS@ \
	@xml2_CFLAGS@ @xslt_CFLAGS@ @exslt_CFLAGS@ \
	-I$(srcdir)/%reldir%/public \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/CPE/public \
	-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
	-I$(top_srcdir)/src/common/public

liboscapsource_la_LIBADD = \
	@bz2_LIBS@ \
	@curl_LIBS@ \
	@xml2_LIBS@ @xslt_LIBS@ @exslt_LIBS@ @PTHREAD_LIBS@

pkginclude_HEADERS =\
	%reldir%/public/oscap_source.h


if CODE_COVERAGE_ENABLED
liboscapsource_la_LIBADD += $(CODE_COVERAGE_LIBS)
liboscapsource_la_CPPFLAGS += $(CODE_COVERAGE_CPPFLAGS)
liboscapsource_la_CFLAGS = $(CODE_COVERAGE_CFLAGS) $(CFLAGS)
endif

libopenscap_la_LIBADD += liboscapsource.la
libopenscap_testing_la_LIBADD += liboscapsource.la
