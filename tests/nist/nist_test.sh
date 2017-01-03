#!/bin/bash

# OpenSCAP test suite
# Tests a subset of NIST validation requirements
# Author:
#     Jan Černý <jcerny@redhat.com>

. ${srcdir}/../test_common.sh

set -e -o pipefail

function test_nist {
	test_dir="$1"
	run_script="$(cd "${builddir}"; pwd)/run"
	# make sure the output dir in builddir exists
	mkdir -p "${builddir}/tests/nist/$test_dir/"
	"${srcdir}/test_worker.py" --scanner "${run_script} $OSCAP" --outputdir "${builddir}/tests/nist/$test_dir/" "${srcdir}/$test_dir/"
	ret_val=$?
	if [ $ret_val -eq 1 ]; then
		return 1
	fi
}

function test_results_directive {
	test_dir="$1"
	run_script="$(cd "${builddir}"; pwd)/run"
	# make sure the output dir in builddir exists
	mkdir -p "${builddir}/tests/nist/$test_dir/"
	local datastream="ind_family_test-datastream.xml"

	local result="${builddir}/tests/nist/${test_dir}/results_directive.results_arf.xml"
	# the tests of this evaluation dont need to pass, return 0
	${run_script} $OSCAP xccdf eval --profile "xccdf_gov.nist_profile_family-validation" --results-arf ${result} "${srcdir}/$test_dir/${datastream}" > /dev/null && 0

	assert_exists 1 '//collected_objects'
	assert_exists 1 '//system_data'
	assert_exists 8 '//tests/test/tested_item'
	assert_exists 9 '//results/system/definitions/definition/criteria'
	if grep -q 'content="full"' ${builddir}/tests/nist/${test_dir}/results_directive.results_arf.xml; then
		echo "ARF results for Single Machine with System Characteristics: PASS"
	else
		echo "ARF results for Single Machine with System Characteristics: FAIL"
		return 1
	fi

	local result="${builddir}/tests/nist/${test_dir}/without-syschar-results_directive.results_arf.xml"
	# the tests of this evaluation dont need to pass, return 0
	${run_script} $OSCAP xccdf eval --profile "xccdf_gov.nist_profile_family-validation" --without-syschar --results-arf ${result} "${srcdir}/$test_dir/${datastream}" > /dev/null && 0

	assert_exists 0 '//collected_objects'
	assert_exists 0 '//system_data'
	assert_exists 8 '//tests/test/tested_item'
	assert_exists 9 '//results/system/definitions/definition/criteria'
	if grep -q 'content="full"' ${builddir}/tests/nist/${test_dir}/without-syschar-results_directive.results_arf.xml; then
		echo "ARF results for Single Machine without System Characteristics: PASS"
	else
		echo "ARF results for Single Machine without System Characteristics: FAIL"
		return 1
	fi

	local result="${builddir}/tests/nist/${test_dir}/thin-results-results_directive.results_arf.xml"
	# the tests of this evaluation dont need to pass, return 0
	${run_script} $OSCAP xccdf eval --profile "xccdf_gov.nist_profile_family-validation" --thin-results --results-arf ${result} "${srcdir}/$test_dir/${datastream}" > /dev/null && 0

	assert_exists 0 '//collected_objects'
	assert_exists 0 '//system_data'
	assert_exists 0 '//tests/test'
	assert_exists 0 '//tests/test/tested_item'
	assert_exists 9 '//results/system/definitions/definition'
	assert_exists 0 '//results/system/definitions/definition/criteria'
	if grep -q 'content="thin"' ${builddir}/tests/nist/${test_dir}/thin-results-results_directive.results_arf.xml; then
		echo "ARF results for Single Machine with Thin Results: PASS"
	else
		echo "ARF results for Single Machine with Thin Results: FAIL"
		return 1
	fi
}

# Testing
test_init "nist_test.log"
test_run "ind_family_test" test_nist "ind_family_test"
test_run "ind_unknown_test" test_nist "ind_unknown_test"
test_run "ind_variable_test" test_nist "ind_variable_test"
test_run "R500" test_nist "R500"
test_run "R600" test_nist "R600"
test_run "R1100" test_nist "R1100"
test_run "R1200" test_nist "R1200"
test_run "R2000" test_results_directive "R2000"
test_run "R2920" test_nist "R2940"
test_run "R3005" test_nist "R3005"
test_run "R3010" test_nist "R3010"
test_run "R3300" test_nist "R3300"
test_exit

