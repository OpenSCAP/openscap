EXTRA_DIST = $(top_srcdir)/tests/test_common.sh

DISTCLEANFILES = *.log *.results oscap_debug.log.*
CLEANFILES = *.log *.results oscap_debug.log.*

# We use absolute paths for LD_* because relative paths would break tests
# whenever cd/pushd is used in them.
TESTS_ENVIRONMENT=OVAL_PROBE_DIR=$(shell cd $(top_builddir); pwd)/src/OVAL/probes \
                  LD_PRELOAD=$(shell cd $(top_builddir); pwd)/src/.libs/libopenscap_testing.so \
                  LD_LIBRARY_PATH=$(shell cd $(top_builddir); pwd)/src/.libs \
                  OSCAP_SCHEMA_PATH=$(shell cd $(top_srcdir); pwd)/schemas \
				  OSCAP_CPE_PATH=$(shell cd $(top_srcdir); pwd)/cpe \
		builddir=$(top_builddir) \
                  OSCAP_FULL_VALIDATION=1

TESTS = test_ds.sh

EXTRA_DIST += test_ds.sh \
		eval_invalid/sds.xml \
		eval_invalid/sds-oval.xml \
		eval_simple/sds.xml \
		eval_just_oval/sds.xml \
		eval_xccdf_id/sds.xml \
		eval_xccdf_id/sds-complex.xml \
		eval_oval_id/sds.xml \
		eval_cpe/sds.xml \
		rds_simple/results-oval.xml \
		rds_simple/results-xccdf.xml \
		rds_simple/sds.xml \
		rds_testresult/results-oval.xml \
		rds_testresult/results-xccdf.xml \
		rds_testresult/sds.xml \
		sds_extended_component/check.xml \
		sds_extended_component/fake-check-xccdf.xml \
		sds_extended_component/scap-fedora14-oval.xml \
		sds_multiple_oval/first-oval.xml \
		sds_multiple_oval/second-oval.xml \
		sds_multiple_oval/multiple-oval-xccdf.xml \
		sds_simple/scap-fedora14-oval.xml \
		sds_simple/scap-fedora14-xccdf.xml \
		sds_subdir/subdir/scap-fedora14-oval.xml \
		sds_subdir/subdir/scap-fedora14-xccdf.xml

SUBDIRS = ds_sds_index signed validate
