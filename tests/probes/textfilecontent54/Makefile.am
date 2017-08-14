DISTCLEANFILES = \
	*.log \
	oscap_debug.log.* \
	results.xml
CLEANFILES = \
	*.log \
	oscap_debug.log.* \
	*results.xml

TESTS_ENVIRONMENT = \
		builddir=$(top_builddir) \
		OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = all.sh

EXTRA_DIST = \
	all.sh \
	test_probes_textfilecontent54.sh \
	test_probes_textfilecontent54.xml \
	test_validation_of_various_oval_versions.sh \
	test_symlinks.sh \
	test_symlinks.xml.tpl \
	test_behavior_multiline.sh \
	test_behavior_multiline.xml.tpl \
	tfc54-def-5.4-invalid.xml \
	tfc54-def-5.4-valid.xml \
	tfc54-def-5.5-valid.xml \
	tfc54-def-5.6-valid.xml \
	tfc54-def-5.7-valid.xml \
	tfc54-def-5.8-valid.xml \
	tfc54-def-5.9-valid.xml \
	tfc54-def-5.10-valid.xml \
	tfc54-def-5.10.1-valid.xml
