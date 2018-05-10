noinst_LTLIBRARIES += liboscapcommon.la

liboscapcommon_la_SOURCES = \
	%reldir%/alloc.c %reldir%/alloc.h \
	%reldir%/assume.h \
	%reldir%/bfind.c %reldir%/bfind.h \
	%reldir%/debug.c %reldir%/debug_priv.h \
	%reldir%/elements.c %reldir%/elements.h \
	%reldir%/err_queue.c %reldir%/err_queue.h \
	%reldir%/error.c %reldir%/_error.h \
	%reldir%/list.c %reldir%/list.h \
	%reldir%/memusage.c %reldir%/memusage.h \
	%reldir%/oscap_acquire.c %reldir%/oscap_acquire.h \
	%reldir%/oscapxml.c %reldir%/oscapxml.h \
	%reldir%/oscap_buffer.c %reldir%/oscap_buffer.h \
	%reldir%/oscap_string.c %reldir%/oscap_string.h \
	%reldir%/reference.c %reldir%/reference_priv.h \
	%reldir%/text.c %reldir%/text_priv.h \
	%reldir%/tsort.c %reldir%/tsort.h \
	%reldir%/util.c %reldir%/util.h \
	%reldir%/xml_iterate.c %reldir%/xml_iterate.h \
	%reldir%/xmlns_priv.h \
	%reldir%/xmltext_priv.c %reldir%/xmltext_priv.h

liboscapcommon_la_CPPFLAGS  = \
	@curl_CFLAGS@ \
	@xml2_CFLAGS@ @xslt_CFLAGS@ @exslt_CFLAGS@ \
	-I$(srcdir)/%reldir%/public \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
	-I$(top_srcdir)/src/source/public

liboscapcommon_la_LIBADD = \
	@curl_LIBS@ \
	@xml2_LIBS@ @xslt_LIBS@ @exslt_LIBS@ @PTHREAD_LIBS@

if CODE_COVERAGE_ENABLED
liboscapcommon_la_LIBADD += $(CODE_COVERAGE_LIBS)
liboscapcommon_la_CPPFLAGS += $(CODE_COVERAGE_CPPFLAGS)
liboscapcommon_la_CFLAGS = $(CODE_COVERAGE_CFLAGS) $(CFLAGS)
endif

pkginclude_HEADERS += \
	%reldir%/public/oscap_debug.h \
	%reldir%/public/oscap_error.h \
	%reldir%/public/oscap.h \
	%reldir%/public/oscap_reference.h \
	%reldir%/public/oscap_download_cb.h \
	%reldir%/public/oscap_text.h

libopenscap_la_LIBADD += liboscapcommon.la
libopenscap_testing_la_LIBADD += liboscapcommon.la
