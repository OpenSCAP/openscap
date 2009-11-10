noinst_LTLIBRARIES = libcvss.la
libcvss_la_SOURCES = cvss.c cvss_priv.c \
		     cvss_priv.h

libcvss_la_CPPFLAGS  = @libxml_cflags@ -Ipublic
libcvss_la_LDFLAGS = @libxml_libs@

pkginclude_HEADERS = public/cvss.h

