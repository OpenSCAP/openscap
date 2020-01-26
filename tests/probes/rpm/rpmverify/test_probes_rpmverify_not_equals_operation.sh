#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail


function perform_test {
    probecheck "rpmverify" || return 255

    DF="$srcdir/test_probes_rpmverify_not_equals_operation.xml"
    RF="results.xml"

    rm -f $RF

    $OSCAP oval eval --results $RF $DF

    result=$RF

    $OSCAP oval validate --results $result

    assert_exists 1 'oval_results/results/system/tests/test[@test_id="oval:x:tst:1"][@result="true"]'
    sc='oval_results/results/system/oval_system_characteristics/'
    sd=$sc'system_data/'
    assert_exists 2 $sc'collected_objects/object'
    assert_exists 2 $sc'collected_objects/object[@flag="complete"]'
    assert_exists 1 $sc'collected_objects/object[@id="oval:x:obj:1"]'
    assert_exists 1 $sc'collected_objects/object[@id="oval:x:obj:2"]'

    rm -f $RF
}


test_init

test_run "rpmverify probe test not equals operation" perform_test

test_exit