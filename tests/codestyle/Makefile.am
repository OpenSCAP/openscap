
EXTRA_DIST = $(top_srcdir)/tests/test_common.sh
	     $(top_srcdir)/tests/assume.h

DISTCLEANFILES = *.log *.out* oscap_debug.log.* $(check_DATA)
CLEANFILES = *.log *.out* oscap_debug.log.* $(check_DATA)

TESTS_ENVIRONMENT= \
		top_srcdir=$(top_srcdir) \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST += \
	all.sh
