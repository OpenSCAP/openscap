#!/usr/bin/env bash
# Regression test for NULL-pointer dereferences on malformed OVAL input.
# Each case used to crash (SIGSEGV) before the NULL guards were added.

. $builddir/tests/test_common.sh

result=0

# Missing version attribute on a state caused atoi(NULL) in oval_state_parse.
assert_no_crash "oval_state version=NULL" \
    info "$srcdir/test_null_ptr_regression.oval.xml" || result=1

# Duplicate variable ID yielded a NULL varModel frame, then dereferenced it.
assert_no_crash "oval_varModel duplicate ID" \
    info "$srcdir/test_null_ptr_regression_varmodel.oval_variables.xml" || result=1

exit $result
