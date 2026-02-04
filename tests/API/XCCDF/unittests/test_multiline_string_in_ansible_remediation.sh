#!/usr/bin/env bash
. $builddir/tests/test_common.sh

# Test XCCDF values with multiline strings are correctly processed when generating Ansible remediation Playbooks

set -e
set -o pipefail

ds="$srcdir/test_multiline_string_in_ansible_remediation_ds.xml"

function test_oscap() {
    local variant="$1"
    local raw_output="$(mktemp)"
    local no_header_output="$(mktemp)"
    local stdout="$(mktemp)"
    local stderr="$(mktemp)"
    $OSCAP xccdf generate fix --profile "xccdf_com.example.www_profile_test_$variant" --fix-type ansible --output "$raw_output" "$ds" >"$stdout" 2>"$stderr"
    [ -f "$stdout" ]
    [ ! -s "$stdout" ]
    [ -f "$stderr" ]
    [ ! -s "$stderr" ]
    sed '/^#/d' "$raw_output" > "$no_header_output"
    diff -u "$no_header_output" "$srcdir/test_multiline_string_in_ansible_remediation_playbook_$variant.yml"
    rm "$raw_output"
    rm "$no_header_output"
    rm "$stdout"
    rm "$stderr"
}

test_oscap "single_line_string"
test_oscap "multi_line_string"
