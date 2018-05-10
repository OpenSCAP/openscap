DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

TESTS_ENVIRONMENT= \
		OSCAP_FULL_VALIDATION=1 \
		builddir=$(top_builddir) \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = all.sh
