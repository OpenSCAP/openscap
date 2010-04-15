noinst_LTLIBRARIES = libcvss.la
libcvss_la_SOURCES = cvss.c cvss_priv.c \
		     cvss_priv.h

# "-ffloat-store" because of http://gcc.gnu.org/ml/gcc/2005-01/msg01082.html
libcvss_la_CPPFLAGS  = @libxml_cflags@ -Ipublic -ffloat-store
libcvss_la_LDFLAGS = @libxml_libs@

pkginclude_HEADERS = public/cvss.h

