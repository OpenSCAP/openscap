include $(top_srcdir)/aminclude_static.am

SUBDIRS =
pkginclude_HEADERS =
lib_LTLIBRARIES = libopenscap.la
noinst_LTLIBRARIES = libopenscap_testing.la

libopenscap_la_SOURCES =
libopenscap_la_LIBADD = $(top_builddir)/lib/libgnu.la
libopenscap_la_LDFLAGS = -no-undefined -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)
libopenscap_la_CFLAGS = $(CFLAGS)

libopenscap_testing_la_SOURCES =
libopenscap_testing_la_LIBADD = $(top_builddir)/lib/libgnu.la
libopenscap_testing_la_LDFLAGS= -rpath /nowhere
libopenscap_testing_la_CFLAGS = $(CFLAGS)

include common/Makefile.am
include source/Makefile.am

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
