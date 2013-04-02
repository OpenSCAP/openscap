DISTCLEANFILES = *.log *.out* $(check_DATA)
CLEANFILES = *.log *.out* $(check_DATA)

TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(top_builddir)/src/OVAL/probes \
		LD_PRELOAD=$(top_builddir)/src/.libs/libopenscap_testing.so \
		LD_LIBRARY_PATH=$(top_builddir)/src/.libs \
		OSCAP_SCHEMA_PATH=$(top_srcdir)/schemas \
		OSCAP_CPE_PATH=$(top_srcdir)/cpe \
		builddir=$(top_builddir)

TESTS = all.sh

EXTRA_DIST = \
	$(top_srcdir)/tests/xpath.pl \
	all.sh \
	applicable-rule-xccdf.xml \
	applicable-rule-embedded-xccdf.xml \
	cpe-dict.xml \
	cpe2-dict.xml \
	cpe-oval.xml \
	cpe2-applicable-rule-xccdf.xml \
	cpe2-applicable-rule-embedded-xccdf.xml \
	cpe2-negated-applicable-rule-embedded-xccdf.xml \
	cpe2-and-rule-embedded-xccdf.xml \
	cpe2-or-rule-embedded-xccdf.xml \
	nonexistant-platforms-rule-xccdf.xml \
	test_remediate_fix_notapplicable.sh \
	test_remediate_fix_notapplicable.xccdf.xml \
	test_remediate_fix_processing.sh \
	test_remediate_fix_processing.xccdf.xml \
	test_remediate_fix_processing_ds.sh \
	test_report_anaconda_fixes.sh \
	test_report_anaconda_fixes.xccdf.xml

