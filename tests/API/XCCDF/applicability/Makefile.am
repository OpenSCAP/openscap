DISTCLEANFILES = *.log *.out* oscap_debug.log.*
CLEANFILES = *.log *.out* oscap_debug.log.*

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	applicable-benchmark-xccdf.xml \
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
	cpe2-applicable-rule-embedded-xccdf-combined.xml \
	cpe2-notapplicable-rule-embedded-xccdf-combined.xml \
	nonexistant-platforms-rule-xccdf.xml \
	openscap-cpe-oval.xml \
	test_platform_element.cpe.xml \
	test_platform_element.sh \
	test_platform_element.xccdf.xml \
	test_remediate_fix_notapplicable.sh \
	test_remediate_fix_notapplicable.xccdf.xml \
	test_remediate_fix_processing.sh \
	test_remediate_fix_processing.xccdf.xml \
	test_remediate_fix_processing_ds.sh \
	test_report_anaconda_fixes.sh \
	test_report_anaconda_fixes.xccdf.xml

