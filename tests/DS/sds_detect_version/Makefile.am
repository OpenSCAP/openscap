DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_detect_version.sh

EXTRA_DIST = test_detect_version.sh \
			 scap-1.2-ds.xml \
			 scap-1.3-ds.xml
