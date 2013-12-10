DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	$(top_srcdir)/tests/xpath.pl \
	all.sh \
	simple-xccdf.xml \
	simple-oval.xml \
	simple-tailoring.xml \
	simple-tailoring11.xml \
	simple-ds.xml
