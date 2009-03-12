SUBDIRS = CPE CVE CCE CVSS OVAL

lib_LTLIBRARIES = libopenscap.la
libopenscap_la_SOURCES = 
libopenscap_la_LIBADD =

if WANT_OVAL
libopenscap_la_LIBADD += OVAL/liboval.la
endif

if WANT_CPE
libopenscap_la_LIBADD += CPE/libcpe.la
endif

if WANT_CVE
libopenscap_la_LIBADD += CVE/libcve.la
endif

if WANT_CCE
libopenscap_la_LIBADD += CCE/libcce.la
endif

if WANT_CVSS
libopenscap_la_LIBADD += CVSS/libcvss.la
endif
