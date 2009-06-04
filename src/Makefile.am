SUBDIRS =
lib_LTLIBRARIES = libopenscap.la
libopenscap_la_SOURCES = 
libopenscap_la_LIBADD =

if WANT_OVAL
SUBDIRS += OVAL
libopenscap_la_LIBADD += OVAL/liboval.la
endif

if WANT_CPE
SUBDIRS += CPE
libopenscap_la_LIBADD += CPE/libcpe.la
endif

if WANT_CVE
SUBDIRS += CVE
libopenscap_la_LIBADD += CVE/libcve.la
endif

if WANT_CCE
SUBDIRS += CCE
libopenscap_la_LIBADD += CCE/libcce.la
endif

if WANT_CVSS
SUBDIRS += CVSS
libopenscap_la_LIBADD += CVSS/libcvss.la
endif

if WANT_XCCDF
SUBDIRS += XCCDF
libopenscap_la_LIBADD += XCCDF/libxccdf.la
endif
