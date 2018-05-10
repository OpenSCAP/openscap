#!/bin/bash

# Test generating results and reports from SCE
# Uses check which produce empty output to test
# retreiving results from an external file.
#
# Author:
#	Jan Černý <jcerny@redhat.com>

. "$builddir/tests/test_common.sh"

set -e -o pipefail

# Test Cases.
function test_check_engine_results {

    xccdf_file=${srcdir}/$1
    stderr=$(mktemp)
    report=$(mktemp)
    results="empty_stdout.sh.result.xml"

    [ -f $results ] && rm -f $results

    $OSCAP xccdf eval --check-engine-results --report $report "$xccdf_file" 2> $stderr
    [ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
    [ -f $results ]; [ -s $results ]; rm $results
    [ -f $report ]; [ -s $report ]; rm $report
}

# Testing.
test_init "test_check_engine_results.log"

test_run "check_engine_results" test_check_engine_results test_check_engine_results.xccdf.xml

test_exit

