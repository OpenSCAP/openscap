DISTCLEANFILES = *.log *.out*
CLEANFILES = *.log *.out*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		$(top_builddir)/run

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
	cpe2-applicable-rule-embedded-xccdf-combined.xml \
	cpe2-notapplicable-rule-embedded-xccdf-combined.xml \
	nonexistant-platforms-rule-xccdf.xml \
	test_remediate_fix_notapplicable.sh \
	test_remediate_fix_notapplicable.xccdf.xml \
	test_remediate_fix_processing.sh \
	test_remediate_fix_processing.xccdf.xml \
	test_remediate_fix_processing_ds.sh \
	test_report_anaconda_fixes.sh \
	test_report_anaconda_fixes.xccdf.xml

