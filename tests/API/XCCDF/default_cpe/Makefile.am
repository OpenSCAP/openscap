DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	$(top_srcdir)/tests/xpath.pl \
	all.sh \
	fedora16-xccdf.xml \
	fedora17-xccdf.xml \
	fedora18-xccdf.xml \
	fedora19-xccdf.xml \
	fedora20-xccdf.xml \
	fedora21-xccdf.xml \
	rhel5-xccdf.xml \
	rhel6-xccdf.xml \
	rhel7-xccdf.xml \
	cpe-oval.xml
