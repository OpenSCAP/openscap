SUBDIRS =
lib_LTLIBRARIES = libopenscap.la
noinst_LTLIBRARIES = libopenscap_testing.la

libopenscap_la_SOURCES =
libopenscap_la_LIBADD = $(top_builddir)/lib/libgnu.la
libopenscap_la_LDFLAGS = -no-undefined -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

libopenscap_testing_la_SOURCES =
libopenscap_testing_la_LIBADD = $(top_builddir)/lib/libgnu.la
libopenscap_testing_la_LDFLAGS= -no-undefined -rpath /nowhere

SUBDIRS += common
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

SUBDIRS += XCCDF
libopenscap_la_LIBADD += XCCDF/libxccdf.la
libopenscap_testing_la_LIBADD += XCCDF/libxccdf.la
####### XCCDF_POLICY submodul ##########
SUBDIRS += XCCDF_POLICY
libopenscap_la_LIBADD += XCCDF_POLICY/libxccdf_policy.la
libopenscap_testing_la_LIBADD += XCCDF_POLICY/libxccdf_policy.la
