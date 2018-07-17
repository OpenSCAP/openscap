#!/bin/bash

set -e -o pipefail

. $builddir/tests/test_common.sh

test_init test_api_xccdf_unittests_python.log

test_run "XCCDF Remediate + python fix" $srcdir/test_remediate_python.sh
test_run "XCCDF Remediate + python fix + xhtml:object" $srcdir/test_remediate_python_subs.sh

test_exit
