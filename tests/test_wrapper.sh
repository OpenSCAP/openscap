#!/usr/bin/env bash

. $builddir/tests/test_common.sh

logfile=$builddir/tests/valgrind_test.log
valgrind_error_code=66
test_script="$1"
$test_script
retval=$?
if [[ $retval = $valgrind_error_code && "$ENABLE_VALGRIND" != "OFF" ]] ; then
    cat "$logfile"
fi
exit $retval
