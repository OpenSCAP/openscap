#!/usr/bin/env bash

. $builddir/tests/test_common.sh

test_script="$1"
$test_script
retval=$?
if [[ $retval = $valgrind_error_code && "$ENABLE_VALGRIND" != "OFF" ]] ; then
    cat "$valgrind_logfile"
fi
exit $retval
