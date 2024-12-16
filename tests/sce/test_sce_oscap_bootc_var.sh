#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_var_set () {
    stdout=$(mktemp)
    stderr=$(mktemp)
    arf=$(mktemp)

    OSCAP_PREFERRED_ENGINE="SCE" OSCAP_BOOTC_BUILD="YES" $OSCAP xccdf eval --verbose INFO --progress --profile common --results-arf "$arf" > "$stdout" 2> "$stderr" "$srcdir/test_sce_oscap_bootc_var.ds.xml" || ret="$?"
    grep -q "xccdf_org.openscap.www_rule_1:fail" "$stdout"
    ! grep -q "I: oscap: Evaluating definition 'oval:org.openscap.www:def:1': OVAL check for rule 1." "$stderr"
    grep -q "I: oscap: Executing SCE check 'fedora/checks/sce/rule_1.sh'" "$stderr"
    grep -q "OSCAP_BOOTC_BUILD=YES" "$arf"
    rm -rf "$stdout" "$stderr" "$arf"
}

function test_var_unset () {
    stdout=$(mktemp)
    stderr=$(mktemp)
    arf=$(mktemp)

    OSCAP_PREFERRED_ENGINE="SCE" $OSCAP xccdf eval --verbose INFO --progress --profile common --results-arf "$arf" > "$stdout" 2> "$stderr" "$srcdir/test_sce_oscap_bootc_var.ds.xml" || ret="$?"
    grep -q "xccdf_org.openscap.www_rule_1:fail" "$stdout"
    ! grep -q "I: oscap: Evaluating definition 'oval:org.openscap.www:def:1': OVAL check for rule 1." "$stderr"
    grep -q "I: oscap: Executing SCE check 'fedora/checks/sce/rule_1.sh'" "$stderr"
    ! grep -q "OSCAP_BOOTC_BUILD=YES" "$arf"
    rm -rf "$stdout" "$stderr" "$arf"
}

test_var_set
test_var_unset
