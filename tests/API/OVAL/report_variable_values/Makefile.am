DISTCLEANFILES = \
	*.log \
	oscap_debug.log.* \
	*\.stderr\.* \
	*\.res\.*\.xml
CLEANFILES = $(DISTCLEANFILES)

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh
EXTRA_DIST = \
	all.sh \
	report_variable_values.def.xml \
	report_variable_values.var.xml
