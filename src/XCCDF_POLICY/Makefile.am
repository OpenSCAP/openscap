noinst_LTLIBRARIES = libxccdf_policy.la

libxccdf_policy_la_SOURCES = xccdf_policy.c


libxccdf_policy_la_CFLAGS = \
	@xml2_CFLAGS@ \
	-I$(top_srcdir)/src/XCCDF/public \
	-I$(top_srcdir)/src/common/public \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/XCCDF_POLICY \
	-I$(top_srcdir)/src/XCCDF \
	-I$(top_srcdir)/src/CPE/public \
	-I$(top_srcdir)/src/OVAL/public

libxccdf_policy_la_LDFLAGS = @xml2_LIBS@

pkginclude_HEADERS  = public/xccdf_policy.h

