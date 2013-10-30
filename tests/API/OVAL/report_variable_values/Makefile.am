DISTCLEANFILES = *.log oscap_debug.log.*
CLEANFILES = oscap_debug.log.*
TESTS_ENVIRONMENT = \
	builddir=$(top_builddir) \
		$(top_builddir)/run
TESTS = all.sh
EXTRA_DIST = \
	$(top_srcdir)/tests/xpath.pl \
	all.sh \
	report_variable_values.def.xml \
	report_variable_values.var.xml
