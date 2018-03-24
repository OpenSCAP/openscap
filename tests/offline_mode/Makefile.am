EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.* *.result.xml */*.results_arf.xml */*.results_oval.xml */*.stdout
CLEANFILES = *.log *.results oscap_debug.log.* *.result.xml */*.results_arf.xml */*.results_oval.xml */*.stdout

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_system_info.sh

EXTRA_DIST += test_system_info.sh \
		grubenv \
		textfilecontent54.oval.xml
