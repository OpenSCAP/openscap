DISTCLEANFILES = *.log *.out* $(check_DATA)
CLEANFILES = *.log *.out* $(check_DATA)

TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
		LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
		LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
		OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
		OSCAP_CPE_PATH=$(top_srcdir)/cpe \
		OSCAP=$(top_builddir)/utils/.libs/oscap

TESTS = all.sh

EXTRA_DIST = \
	$(top_srcdir)/tests/xpath.pl \
	all.sh \
	fedora16-xccdf.xml \
	fedora17-xccdf.xml \
	fedora18-xccdf.xml \
	rhel5-xccdf.xml \
	rhel6-xccdf.xml \
	cpe-oval.xml
