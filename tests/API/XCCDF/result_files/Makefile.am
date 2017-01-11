DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	simple-xccdf.xml \
	cpe-dict.xml \
	cpe-oval.xml
