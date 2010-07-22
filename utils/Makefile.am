

if WANT_UTIL_OSCAP
bin_PROGRAMS	= oscap
man_MANS	= oscap.8
oscap_SOURCES	= oscap.c oscap-tool.h
oscap_CPPFLAGS	= -I$(top_srcdir)/src/common/public @curl_cflags@
oscap_LDADD	= $(top_builddir)/src/libopenscap.la @curl_libs@
EXTRA_DIST	= $(man_MANS)


if WANT_PROBES
oscap_CPPFLAGS	+= -DENABLE_OVAL
oscap_SOURCES	+= oscap-oval.c
oscap_CPPFLAGS	+= -I$(top_srcdir)/src/OVAL/public

if WANT_XCCDF
oscap_CPPFLAGS	+= -DENABLE_XCCDF
oscap_SOURCES	+= oscap-xccdf.c
oscap_CPPFLAGS	+=-I$(top_srcdir)/src/XCCDF/public \
		  -I$(top_srcdir)/src/XCCDF_POLICY/public
endif
endif


if WANT_CVSS
oscap_CPPFLAGS	+= -DENABLE_CVSS
oscap_SOURCES	+= oscap-cvss.c
oscap_CPPFLAGS	+= -I$(top_srcdir)/src/CVSS/public
endif


endif

