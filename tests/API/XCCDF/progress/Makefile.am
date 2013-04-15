DISTCLEANFILES = *.log *.out* $(check_DATA)
CLEANFILES = *.log *.out* $(check_DATA)

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	content-xccdf11.xml \
	content-xccdf12.xml \
	content-oval.xml
