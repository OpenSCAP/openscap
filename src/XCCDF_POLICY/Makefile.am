noinst_LTLIBRARIES = libxccdf_policy.la

libxccdf_policy_la_SOURCES = \
	reporter.c \
	reporter_priv.h \
	xccdf_policy.c \
	xccdf_policy_engine.c \
	xccdf_policy_engine_priv.h \
	xccdf_policy_model_priv.h \
	xccdf_policy_priv.h \
	xccdf_policy_remediate.c \
	xccdf_policy_substitute.c \
	check_engine_plugin.c

libxccdf_policy_la_CFLAGS = \
	@xml2_CFLAGS@ \
	-I$(top_srcdir)/src/XCCDF/public \
	-I$(top_srcdir)/src/common/public \
	-I$(top_srcdir)/src/source/public \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/XCCDF_POLICY \
	-I$(top_srcdir)/src/XCCDF \
	-I$(top_srcdir)/src/CPE/public \
	-I$(top_srcdir)/src/OVAL/public

libxccdf_policy_la_LDFLAGS = @xml2_LIBS@

pkginclude_HEADERS  = public/xccdf_policy.h \
	public/check_engine_plugin.h

