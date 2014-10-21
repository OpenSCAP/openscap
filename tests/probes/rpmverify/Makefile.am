DISTCLEANFILES = \
	*.log \
	oscap_debug.log.* \
	*.out.??????
CLEANFILES = $(DISTCLEANFILES)

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

if WANT_PROBES_LINUX
if probe_rpmverify_enabled
TESTS = all.sh
endif
endif

check_SCRIPTS = test_not_equals_operation.sh

EXTRA_DIST = \
	all.sh \
	test_not_equals_operation.sh \
	test_not_equals_operation.xml
