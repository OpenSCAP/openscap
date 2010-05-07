noinst_LTLIBRARIES = libxccdf_policy.la
libxccdf_policy_la_SOURCES = \
	xccdf_policy.c
	

libxccdf_policy_la_CFLAGS  = @libxml_cflags@ -I$(top_srcdir)/src/common/public \
			     		     -I$(top_srcdir)/src/XCCDF_POLICY/public \
					     -I$(top_srcdir)/src/XCCDF/public \
					     -I$(top_srcdir)/src/OVAL/public
libxccdf_policy_la_LDFLAGS = @libxml_libs@

pkginclude_HEADERS  = public/xccdf_policy.h

