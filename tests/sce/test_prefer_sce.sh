#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail


function test_prefer_sce_on () {
    stdout=$(mktemp)
    stderr=$(mktemp)
    arf=$(mktemp)

    OSCAP_PREFERRED_ENGINE="SCE" $OSCAP xccdf eval --verbose INFO --progress --profile common --results-arf "$arf" > "$stdout" 2> "$stderr" "$srcdir/test_prefer_sce.ds.xml" || ret="$?"
    grep -q "xccdf_org.openscap.www_rule_1:fail" "$stdout"
    ! grep -q "I: oscap: Evaluating definition 'oval:org.openscap.www:def:1': OVAL check for rule 1." "$stderr"
    grep -q "I: oscap: Executing SCE check 'fedora/checks/sce/rule_1.sh'" "$stderr"

    rm -rf "$stdout" "$stderr" "$arf"
}

test_prefer_sce_off () {
    stdout=$(mktemp)
    stderr=$(mktemp)
    arf=$(mktemp)

    $OSCAP xccdf eval --verbose INFO --progress --profile common --results-arf "$arf" > "$stdout" 2> "$stderr" "$srcdir/test_prefer_sce.ds.xml" || ret="$?"
    grep -q "xccdf_org.openscap.www_rule_1:pass" "$stdout"
    grep -q "I: oscap: Evaluating definition 'oval:org.openscap.www:def:1': OVAL check for rule 1." "$stderr"
    ! grep -q "I: oscap: Executing SCE check 'fedora/checks/sce/rule_1.sh'" "$stderr"

    rm -rf "$stdout" "$stderr" "$arf"
}

test_prefer_oval_explicit () {
    stdout=$(mktemp)
    stderr=$(mktemp)
    arf=$(mktemp)

    OSCAP_PREFERRED_ENGINE="OVAL" $OSCAP xccdf eval --verbose INFO --progress --profile common --results-arf "$arf" > "$stdout" 2> "$stderr" "$srcdir/test_prefer_sce.ds.xml" || ret="$?"
    grep -q "xccdf_org.openscap.www_rule_1:pass" "$stdout"
    grep -q "I: oscap: Evaluating definition 'oval:org.openscap.www:def:1': OVAL check for rule 1." "$stderr"
    ! grep -q "I: oscap: Executing SCE check 'fedora/checks/sce/rule_1.sh'" "$stderr"

    rm -rf "$stdout" "$stderr" "$arf"
}

test_invalid_envi_variable () {
    stdout=$(mktemp)
    stderr=$(mktemp)
    arf=$(mktemp)

    OSCAP_PREFERRED_ENGINE="FOOBARVIM" $OSCAP xccdf eval --verbose INFO --progress --profile common --results-arf "$arf" > "$stdout" 2> "$stderr" "$srcdir/test_prefer_sce.ds.xml" || ret="$?"
    grep -q "Unknown value of OSCAP_PREFFERED_ENGINE: 'FOOBARVIM'. It will be ignored." "$stderr"
    grep -q "xccdf_org.openscap.www_rule_1:pass" "$stdout"
    grep -q "I: oscap: Evaluating definition 'oval:org.openscap.www:def:1': OVAL check for rule 1." "$stderr"
    ! grep -q "I: oscap: Executing SCE check 'fedora/checks/sce/rule_1.sh'" "$stderr"

    rm -rf "$stdout" "$stderr" "$arf"
}

test_prefer_sce_on
test_prefer_sce_off
test_prefer_oval_explicit
test_invalid_envi_variable
