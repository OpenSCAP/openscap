EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.* *.result.xml *.results_arf.xml */*.results_oval.xml */*.stdout
CLEANFILES = *.log *.results oscap_debug.log.* *.result.xml *.results_arf.xml */*.results_oval.xml */*.stdout

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
		$(top_builddir)/run

TESTS = nist_test.sh

EXTRA_DIST += nist_test.sh \
	test_worker.py \
	ind_family_test/catalog.xml \
	ind_family_test/ind_family_test-datastream.xml \
	ind_unknown_test/catalog.xml \
	ind_unknown_test/ind_unknown_test-datastream.xml \
	ind_variable_test/catalog.xml \
	ind_variable_test/ind_variable_test-datastream.xml \
	R500/catalog.xml \
	R500/r500-datastream.xml \
	R600/catalog.xml \
	R600/r600-datastream.xml \
	R700/catalog.xml \
	R700/configuration_scripts \
	R700/configuration_scripts/text_file_content_test_cleanup.py \
	R700/configuration_scripts/text_file_test_config_1.py \
	R700/configuration_scripts/text_file_test_config_2.py \
	R700/r700-datastream.xml \
	R1100/catalog.xml \
	R1100/r1100-scap10-rhel-cpe-dictionary.xml \
	R1100/r1100-scap10-rhel-cpe-oval.xml \
	R1100/r1100-scap10-rhel-oval.xml \
	R1100/r1100-scap10-rhel-patches.xml \
	R1100/r1100-scap10-rhel-xccdf.xml \
	R1100/r1100-scap11-win_rhel-cpe-dictionary.xml \
	R1100/r1100-scap11-win_rhel-cpe-oval.xml \
	R1100/r1100-scap11-win_rhel-ocil.xml \
	R1100/r1100-scap11-win_rhel-oval.xml \
	R1100/r1100-scap11-win_rhel-patches.xml \
	R1100/r1100-scap11-win_rhel-xccdf.xml \
	R1200/catalog.xml \
	R1200/r1200-datastream.xml \
	R2920/catalog.xml \
	R2920/r2920-datastream.xml \
	R2940/catalog.xml \
	R2940/r2940-datastream.xml \
	R3005/catalog.xml \
	R3005/r3005-datastream-01.xml \
	R3005/r3005-datastream-02.xml \
	R3005/r3005-datastream-03.xml \
	R3010/catalog.xml \
	R3010/r3010-datastream.xml \
	R3300/catalog.xml \
	R3300/r3300-datastream.xml

