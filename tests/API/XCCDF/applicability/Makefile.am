DISTCLEANFILES = *.log *.out* $(check_DATA)
CLEANFILES = *.log *.out* $(check_DATA)

TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
		LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
		LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
		OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
		OSCAP=$(top_builddir)/utils/.libs/oscap

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	applicable-rule-xccdf.xml \
	cpe-dict.xml \
	cpe-oval.xml \
	nonexistant-platforms-rule-xccdf.xml \
	cpe2-applicable-rule-xccdf.xml \
	cpe2-and-rule-xccdf.xml
