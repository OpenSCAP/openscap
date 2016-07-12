#!/usr/bin/env bash

# Runs valgrind tests.
#
# Authors:
#      Jan Černý <jcerny@redhat.com>

# set up variables
oscap_program=$actualdir/utils/.libs/oscap
valgrind_output=/tmp/valgrind_$$.log
logfile=$actualdir/tests/valgrind_test.log
error_code=66
suppfile=$actualdir/tests/suppressions.supp
valgrind_args="--trace-children=yes --free-fill=55 --malloc-fill=55 --leak-check=full --show-reachable=yes --show-leak-kinds=all --log-file=$valgrind_output.%p --suppressions=$suppfile --error-exitcode=$error_code"


echo "VALGRIND TEST" >> $logfile
echo "working directory: " `pwd` >> $logfile
echo "command: oscap $@" >> $logfile

# run valgrind
valgrind $valgrind_args $oscap_program "$@" 
ret_val=$?

if [ $ret_val -eq $error_code ] ; then
    echo "Memory error detected!" >> $logfile
    cat $valgrind_output* >> $logfile
    result=1 # fail the test
else
    echo "OK" >> $logfile
    result=0 # pass the test
fi

echo "" >> $logfile
rm $valgrind_output*
exit $result
