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


# Testing
test_init "nist_test.log"
test_run "ind_family_test" test_nist "ind_family_test"
test_run "ind_unknown_test" test_nist "ind_unknown_test"
test_run "ind_variable_test" test_nist "ind_variable_test"
test_run "R500" test_nist "R500"
test_run "R600" test_nist "R600"
test_run "R1100" test_nist "R1100"
test_run "R1200" test_nist "R1200"
test_run "R2920" test_nist "R2940"
test_run "R3005" test_nist "R3005"
test_run "R3010" test_nist "R3010"
test_run "R3300" test_nist "R3300"
test_exit

