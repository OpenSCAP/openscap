#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_pass_env_var_down() {
    stdout=$(mktemp)
    stderr=$(mktemp)
    result=$(mktemp)

    OSCAP_BOOTC_BUILD=YES $OSCAP xccdf eval --remediate --results "$result" "$srcdir/test_oscap_bootc_pass_down.ds.xml" > "$stdout" 2> "$stderr" || ret=$?
    assert_exists 1 '//rule-result/message[text()="WE ARE BUILDING BOOTABLE CONTAINER IMAGE NOW"]'

    rm -rf "$stdout" "$stderr" "$result"
}

function test_no_env_var() {
    stdout=$(mktemp)
    stderr=$(mktemp)
    result=$(mktemp)

    $OSCAP xccdf eval --remediate --results "$result" "$srcdir/test_oscap_bootc_pass_down.ds.xml" > "$stdout" 2> "$stderr" || ret=$?
    assert_exists 0 '//rule-result/message[text()="WE ARE BUILDING BOOTABLE CONTAINER IMAGE NOW"]'

    rm -rf "$stdout" "$stderr" "$result"
}


test_pass_env_var_down
test_no_env_var
