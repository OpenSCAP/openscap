SUBDIRS =
lib_LTLIBRARIES = libopenscap.la
check_LTLIBRARIES= libopenscap_testing.la

libopenscap_la_SOURCES = 
libopenscap_la_LIBADD =

libopenscap_testing_la_SOURCES = 
libopenscap_testing_la_LIBADD =

SUBDIRS += common
libopenscap_la_LIBADD += common/liboscapcommon.la
libopenscap_testing_la_LIBADD += common/liboscapcommon.la

if WANT_OVAL
SUBDIRS += OVAL
libopenscap_la_LIBADD += OVAL/liboval.la
libopenscap_testing_la_LIBADD += OVAL/liboval_testing.la

endif

if WANT_CPE
SUBDIRS += CPE
libopenscap_la_LIBADD += CPE/libcpe.la
libopenscap_testing_la_LIBADD += CPE/libcpe.la

endif

if WANT_CVE
SUBDIRS += CVE
libopenscap_la_LIBADD += CVE/libcve.la
libopenscap_testing_la_LIBADD += CVE/libcve.la

endif

if WANT_CCE
SUBDIRS += CCE
libopenscap_la_LIBADD += CCE/libcce.la
libopenscap_testing_la_LIBADD += CCE/libcce.la

endif

if WANT_CVSS
SUBDIRS += CVSS
libopenscap_la_LIBADD += CVSS/libcvss.la
libopenscap_testing_la_LIBADD += CVSS/libcvss.la

endif

if WANT_XCCDF
SUBDIRS += XCCDF
libopenscap_la_LIBADD += XCCDF/libxccdf.la
libopenscap_testing_la_LIBADD += XCCDF/libxccdf.la

endif
