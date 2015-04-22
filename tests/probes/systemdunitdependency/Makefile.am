DISTCLEANFILES = \
	*.log \
	oscap_debug.log.* \
	results.xml
CLEANFILES = \
	*.log \
	oscap_debug.log.* \
	*results.xml

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	test_probes_systemdunitdependency.sh \
	test_probes_systemdunitdependency.xml \
	test_validation.sh
