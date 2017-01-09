DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	fedora17-xccdf.xml \
	fedora18-xccdf.xml \
	fedora19-xccdf.xml \
	fedora20-xccdf.xml \
	fedora21-xccdf.xml \
	fedora23-xccdf.xml \
	fedora24-xccdf.xml \
	rhel5-xccdf.xml \
	rhel6-xccdf.xml \
	rhel7-xccdf.xml \
	cpe-oval.xml
