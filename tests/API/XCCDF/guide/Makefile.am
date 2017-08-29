DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = benchmark.sh guide.sh profile.sh

EXTRA_DIST = \
	benchmark.sh \
	guide.sh \
	profile.sh \
	content-xccdf11.xml \
	content-xccdf12.xml \
	content-profile-xccdf11.xml \
	benchmark_id_test-ds.xml
