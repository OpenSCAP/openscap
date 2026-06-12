#!/usr/bin/env bash
# Regression test for NULL-pointer dereferences on malformed XCCDF input.
# Each case used to crash (SIGSEGV) before the NULL guards were added.

. $builddir/tests/test_common.sh

result=0

# Empty <TestResult> element caused NULL item dereference in xccdf_benchmark_add_result.
assert_no_crash "XCCDF empty TestResult" \
    info "$srcdir/test_null_ptr_xccdf_empty_testresult.xml" || result=1

# <platform> with no idref attribute caused strlen(NULL) in xccdf_item_add_applicable_platform.
assert_no_crash "XCCDF platform missing idref" \
    info "$srcdir/test_null_ptr_xccdf_platform_no_idref.xml" || result=1

# Empty <Profile> in tailoring caused NULL dereference in xccdf_tailoring_add_profile.
assert_no_crash "XCCDF tailoring empty profile" \
    info "$srcdir/test_null_ptr_xccdf_tailoring_empty_profile.xml" || result=1

exit $result
