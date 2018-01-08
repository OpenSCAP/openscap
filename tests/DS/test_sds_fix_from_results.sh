#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

set -e -o pipefail

. $builddir/tests/test_common.sh

# Test Cases.

function test_generate_fix_results {
    local fixfile=$(mktemp -t ${name}.out.XXXXXX)
    local results=$(mktemp -t ${name}.out.XXXXXX)

    # generate all from results
    $OSCAP xccdf eval --results $results "${srcdir}/$1" || ret=$?
    $OSCAP xccdf generate fix --output $fixfile $results
    grep -q remediation_rule_applicable_pass $fixfile
    grep -q remediation_rule_applicable_fail $fixfile
    grep -q remediation_rule_notapplicable $fixfile
    rm $fixfile

    # generate based on TestResult
    $OSCAP xccdf generate fix --output $fixfile --result-id xccdf_org.open-scap_testresult_default-profile $results
    grep -qv remediation_rule_applicable_pass $fixfile
    grep -q remediation_rule_applicable_fail $fixfile
    grep -qv remediation_rule_notapplicable $fixfile
    rm $fixfile $results
}

# Testing.
test_init "test_sds_fix_from_results.log"

test_run "generate_fix_cpe_results" test_generate_fix_results eval_cpe/sds.xml

test_exit
