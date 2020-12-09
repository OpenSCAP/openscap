#!/usr/bin/env bash


# Runs valgrind tests.
#
# Authors:
#      Jan Černý <jcerny@redhat.com>

# set up variables
logfile=$builddir/tests/valgrind_test.log
error_code=66
suppfile=$top_srcdir/tests/suppressions.supp
valgrind_args="--trace-children=yes --free-fill=55 --malloc-fill=55 --leak-check=full --show-reachable=yes --show-leak-kinds=all --log-file=$logfile --suppressions=$suppfile --error-exitcode=$error_code"

rm -f "$logfile"

# run valgrind
"$RUN_WRAPPER" valgrind $valgrind_args "$OSCAP_EXEC" "$@"
ret_val=$?
exit $ret_val

exit $result
