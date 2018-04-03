DISTCLEANFILES = \
	*.log \
	oscap_debug.log.* \
	*.out.??????
CLEANFILES = $(DISTCLEANFILES)

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

if WANT_PROBES_UNIX
if probe_sysctl_enabled
TESTS = all.sh
endif
endif

check_SCRIPTS = \
	test_sysctl_probe.sh \
	test_sysctl_probe_all.sh

EXTRA_DIST = \
	all.sh \
	test_sysctl_probe.sh \
	test_sysctl_probe.oval.xml \
	test_sysctl_probe_all.sh \
	test_sysctl_probe_all.oval.xml
