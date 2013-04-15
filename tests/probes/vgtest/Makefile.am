EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.vglog
CLEANFILES = *.log *.vglog

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_probes_vg.sh

EXTRA_DIST += test_probes_vg.sh vgrun.sh openscap.supp
