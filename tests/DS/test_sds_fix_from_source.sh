#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

set -e -o pipefail

. $builddir/tests/test_common.sh

# Test Cases.

function test_generate_fix_source {
    local fixfile=$(mktemp -t ${name}.out.XXXXXX)

    # all rules (default profile)
    $OSCAP xccdf generate fix --output $fixfile "${srcdir}/$1"
    grep -q remediation_rule_applicable_pass $fixfile
    grep -q remediation_rule_applicable_fail $fixfile
    grep -q remediation_rule_notapplicable $fixfile
    rm $fixfile

    # selected profile
    $OSCAP xccdf generate fix --output $fixfile --profile xccdf_org.ssgproject.content_profile_test "${srcdir}/$1"
    grep -qv remediation_rule_applicable_pass $fixfile
    grep -q remediation_rule_applicable_fail $fixfile
    grep -q remediation_rule_notapplicable $fixfile
    rm $fixfile
}

# Testing.
test_init

test_run "generate_fix_cpe_source" test_generate_fix_source eval_cpe/sds.xml

test_exit
