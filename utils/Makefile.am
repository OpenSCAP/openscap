bin_PROGRAMS = oscap-scan

oscap_scan_SOURCES = oscap-scan.c
oscap_scan_CPPFLAGS = -I$(top_srcdir)/src/OVAL/public -I$(top_srcdir)/src/common/public @curl_cflags@
oscap_scan_LDADD = $(top_builddir)/src/libopenscap.la @curl_libs@

