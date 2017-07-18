noinst_LTLIBRARIES = libcvrf.la

libcvrf_la_SOURCES = cvrf.c cvrf_priv.c \
		    cvrf_priv.h cvrf_eval.c
libcvrf_la_CPPFLAGS  = @xml2_CFLAGS@	-I${srcdir}/public \
					-I$(top_srcdir)/src \
					-I$(top_srcdir)/src/common/public \
		      			-I${top_srcdir}/src/CPE/public \
					-I${top_srcdir}/src/CVSS/public \
					-I$(top_srcdir)/src/source/public
libcvrf_la_LDFLAGS = @xml2_LIBS@

pkginclude_HEADERS = public/cvrf.h
