DISTCLEANFILES = *.log *.results \
	oscap_debug.log.* */oscap_debug.log.* sds_subdir/subdir/oscap_debug.log.*
CLEANFILES = *.log *.results \
	oscap_debug.log.* */oscap_debug.log.* sds_subdir/subdir/oscap_debug.log.*

TESTS_ENVIRONMENT= \
		builddir=$(top_builddir) \
                OSCAP_FULL_VALIDATION=1 \
		$(top_builddir)/run

TESTS = test_ds.sh

EXTRA_DIST = test_ds.sh \
		eval_invalid/sds.xml \
		eval_invalid/sds-oval.xml \
		eval_simple/sds.xml \
		eval_just_oval/sds.xml \
		eval_xccdf_id/sds.xml \
		eval_xccdf_id/sds-complex.xml \
		eval_benchmark_id_conflict/sds.xml \
		eval_oval_id/sds.xml \
		eval_cpe/sds.xml \
		cpe_in_ds/sds.xml \
		rds_simple/results-oval.xml \
		rds_simple/results-xccdf.xml \
		rds_simple/sds.xml \
		rds_index_simple/arf.xml \
		rds_split_simple/report-request.xml \
		rds_split_simple/report.xml \
		rds_testresult/results-oval.xml \
		rds_testresult/results-xccdf.xml \
		rds_testresult/sds.xml \
		sds_external_xccdf/sds.ds.xml \
		sds_external_xccdf/xccdf.sds.xml \
		sds_external_xccdf/xccdf.xml \
		sds_extended_component/check.xml \
		sds_extended_component/fake-check-xccdf.xml \
		sds_extended_component/scap-fedora14-oval.xml \
		sds_extended_component_plain_text/check.txt \
		sds_extended_component_plain_text/fake-check-xccdf.xml \
		sds_extended_component_plain_text/scap-fedora14-oval.xml \
		sds_extended_component_plain_text_entities/check.txt \
		sds_extended_component_plain_text_entities/fake-check-xccdf.xml \
		sds_extended_component_plain_text_entities/scap-fedora14-oval.xml \
		sds_extended_component_plain_text_whitespace/check.txt \
		sds_extended_component_plain_text_whitespace/fake-check-xccdf.xml \
		sds_extended_component_plain_text_whitespace/scap-fedora14-oval.xml \
		sds_missing_oval/first-oval.xml \
		sds_missing_oval/multiple-oval-xccdf.xml \
		sds_multiple_oval/first-oval.xml \
		sds_multiple_oval/second-oval.xml \
		sds_multiple_oval/multiple-oval-xccdf.xml \
		sds_simple/scap-fedora14-oval.xml \
		sds_simple/scap-fedora14-xccdf.xml \
		sds_simple_5_11_1/simple_oval.xml \
		sds_simple_5_11_1/simple_xccdf.xml \
		sds_subdir/subdir/scap-fedora14-oval.xml \
		sds_subdir/subdir/scap-fedora14-xccdf.xml

SUBDIRS = ds_sds_index signed validate
