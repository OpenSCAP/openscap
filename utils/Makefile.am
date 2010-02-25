
bin_PROGRAMS = oscap_cli

oscap_cli_SOURCES = oscap_cli.c
oscap_cli_CPPFLAGS = -I$(top_srcdir)/src/OVAL/public -I$(top_srcdir)/src/common/public
oscap_cli_LDADD = $(top_builddir)/src/libopenscap.la @curl_libs@

