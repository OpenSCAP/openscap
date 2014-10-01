noinst_LTLIBRARIES = libcve.la

libcve_la_SOURCES = cve.c cve_priv.c \
		    cve_priv.h
libcve_la_CPPFLAGS  = @xml2_CFLAGS@	-I${srcdir}/public \
					-I$(top_srcdir)/src \
					-I$(top_srcdir)/src/common/public \
		      			-I${top_srcdir}/src/CPE/public \
					-I${top_srcdir}/src/CVSS/public \
					-I$(top_srcdir)/src/source/public
libcve_la_LDFLAGS = @xml2_LIBS@

pkginclude_HEADERS = public/cve_nvd.h

