DISTCLEANFILES = *.log *.out* $(check_DATA)
CLEANFILES = *.log *.out* $(check_DATA)

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	$(top_srcdir)/tests/xpath.pl \
	all.sh \
	requires_both-oval.xml \
	testing_file_300.xml \
	testing_file_600.xml \
	test_xccdf_variable_instance.xccdf.xml
