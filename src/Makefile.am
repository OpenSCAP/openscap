SUBDIRS =
pkginclude_HEADERS =
lib_LTLIBRARIES = libopenscap.la
noinst_LTLIBRARIES = libopenscap_testing.la

libopenscap_la_SOURCES =
libopenscap_la_LIBADD = $(top_builddir)/lib/libgnu.la
libopenscap_la_LDFLAGS = -no-undefined -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

libopenscap_testing_la_SOURCES =
libopenscap_testing_la_LIBADD = $(top_builddir)/lib/libgnu.la
libopenscap_testing_la_LDFLAGS= -rpath /nowhere

noinst_LTLIBRARIES += liboscapcommon.la

liboscapcommon_la_SOURCES = \
	common/alloc.c common/alloc.h \
	common/assume.h \
	common/bfind.c common/bfind.h \
	common/debug.c common/debug_priv.h \
	common/elements.c common/elements.h \
	common/err_queue.c common/err_queue.h \
	common/error.c common/_error.h \
	common/list.c common/list.h \
	common/memusage.c common/memusage.h \
	common/oscap_acquire.c common/oscap_acquire.h \
	common/oscapxml.c common/oscapxml.h \
	common/oscap_buffer.c common/oscap_buffer.h \
	common/oscap_string.c common/oscap_string.h \
	common/reference.c common/reference_priv.h \
	common/text.c common/text_priv.h \
	common/tsort.c common/tsort.h \
	common/util.c common/util.h \
	common/xml_iterate.c common/xml_iterate.h \
	common/xmlns_priv.h \
	common/xmltext_priv.c common/xmltext_priv.h

liboscapcommon_la_CPPFLAGS  = \
	@curl_CFLAGS@ \
	@xml2_CFLAGS@ @xslt_CFLAGS@ @exslt_CFLAGS@ \
	-I$(srcdir)/common/public \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/OVAL/probes/SEAP/public \
	-I$(top_srcdir)/src/source/public

liboscapcommon_la_LIBADD = \
	@curl_LIBS@ \
	@xml2_LIBS@ @xslt_LIBS@ @exslt_LIBS@ @PTHREAD_LIBS@

pkginclude_HEADERS += \
	common/public/oscap_debug.h \
	common/public/oscap_error.h \
	common/public/oscap.h \
	common/public/oscap_reference.h \
	common/public/oscap_download_cb.h \
	common/public/oscap_text.h

libopenscap_la_LIBADD += common/liboscapcommon.la
libopenscap_testing_la_LIBADD += common/liboscapcommon.la

SUBDIRS += source
libopenscap_la_LIBADD += source/liboscapsource.la
libopenscap_testing_la_LIBADD += source/liboscapsource.la

SUBDIRS += DS
libopenscap_la_LIBADD += DS/libds.la
libopenscap_testing_la_LIBADD += DS/libds.la

SUBDIRS += OVAL
libopenscap_la_LIBADD += OVAL/liboval.la
libopenscap_testing_la_LIBADD += OVAL/liboval_testing.la

SUBDIRS += CPE
libopenscap_la_LIBADD += CPE/libcpe.la
libopenscap_testing_la_LIBADD += CPE/libcpe.la

SUBDIRS += CVE
libopenscap_la_LIBADD += CVE/libcve.la
libopenscap_testing_la_LIBADD += CVE/libcve.la

if WANT_CCE
SUBDIRS += CCE
libopenscap_la_LIBADD += CCE/libcce.la
libopenscap_testing_la_LIBADD += CCE/libcce.la
endif

SUBDIRS += CVSS
libopenscap_la_LIBADD += CVSS/libcvss.la
libopenscap_testing_la_LIBADD += CVSS/libcvss.la

SUBDIRS += CVRF
libopenscap_la_LIBADD += CVRF/libcvrf.la
libopenscap_testing_la_LIBADD += CVRF/libcvrf.la

SUBDIRS += XCCDF
libopenscap_la_LIBADD += XCCDF/libxccdf.la
libopenscap_testing_la_LIBADD += XCCDF/libxccdf.la
####### XCCDF_POLICY submodul ##########
SUBDIRS += XCCDF_POLICY
libopenscap_la_LIBADD += XCCDF_POLICY/libxccdf_policy.la
libopenscap_testing_la_LIBADD += XCCDF_POLICY/libxccdf_policy.la
