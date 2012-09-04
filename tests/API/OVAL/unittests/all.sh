#!/bin/bash


. $srcdir/../../../test_common.sh

test_init test_api_oval_unittests.log
test_run "test applicability_check element" $srcdir/test_applicability_check.sh
test_exit

