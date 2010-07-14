
if WANT_UTIL_OSCAP

if WANT_PROBES

bin_PROGRAMS = oscap
man_MANS = oscap.8

oscap_SOURCES = oscap.c
oscap_CPPFLAGS = -I$(top_srcdir)/src/OVAL/public \
		 -I$(top_srcdir)/src/common/public \
		 -I$(top_srcdir)/src/XCCDF/public \
		 -I$(top_srcdir)/src/XCCDF_POLICY/public \
		 -I$(top_srcdir)/src/CVSS/public \
		 @curl_cflags@

oscap_LDADD = $(top_builddir)/src/libopenscap.la @curl_libs@

if WANT_XCCDF
oscap_CPPFLAGS += -DENABLE_XCCDF
endif

EXTRA_DIST = $(man_MANS)

endif

endif
