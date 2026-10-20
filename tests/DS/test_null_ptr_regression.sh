#!/usr/bin/env bash
# Regression test for NULL-pointer dereferences on malformed DS/ARF/SDS input.
# Each case used to crash (SIGSEGV) before the NULL guards were added.

. $builddir/tests/test_common.sh

result=0

# Relationship element with no type attribute caused strcmp(NULL, ...) in rds.c /
# strncmp(NULL, ...) via oscap_str_startswith in util.h.
assert_no_crash "RDS relationship missing type" \
    info "$srcdir/test_null_ptr_rds_missing_rel_type.xml" || result=1

# Empty data-stream-collection causes ds_sds_index_new_from_source to return NULL
# (no streams found), and xccdf_session_load_xccdf then calls
# ds_sds_index_select_checklist(NULL, ...) which dereferences the NULL index.
# --skip-valid bypasses the early schema-validation return in xccdf_session_load_xccdf
# so that ds_sds_session_select_checklist is actually reached.
assert_no_crash "SDS empty collection" \
    xccdf eval --skip-valid "$srcdir/test_null_ptr_sds_empty_collection.xml" || result=1

# data-stream without id caused strcmp(NULL, datastream_id) in sds_index.c when
# a non-NULL datastream-id is requested. --skip-valid bypasses early validation
# return so that ds_sds_session_select_checklist is actually reached.
assert_no_crash "SDS stream missing id" \
    xccdf eval --skip-valid --datastream-id "regression-test-id" \
    "$srcdir/test_null_ptr_sds_stream_no_id.xml" || result=1

exit $result
