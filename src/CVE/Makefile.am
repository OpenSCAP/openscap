noinst_LTLIBRARIES = libcve.la

libcve_la_SOURCES = cve.c cve_priv.c \
		    cve_priv.h
libcve_la_CPPFLAGS  = @libxml_cflags@ -I${srcdir}/public -I$(top_srcdir)/src/common/public \
		      -I${top_srcdir}/src/CPE/ -I${top_srcdir}/src/CPE/public \
		      -I${top_srcdir}/src/CVSS/ -I${top_srcdir}/src/CVSS/public
libcve_la_LDFLAGS = @libxml_libs@

pkginclude_HEADERS = public/cve.h

