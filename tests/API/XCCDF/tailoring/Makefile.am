DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	simple-xccdf.xml \
	simple-oval.xml \
	simple-tailoring.xml \
	simple-tailoring11.xml \
	simple-tailoring-autonegotiation.xml \
	simple-ds.xml
