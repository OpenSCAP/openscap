#!/bin/bash


. $srcdir/../../../test_common.sh

test_init test_api_oval_unittests.log
test_run "empty filename(pattern match)" $srcdir/test_empty_filename.sh
test_run "deprecated definition" $srcdir/test_deprecated_def.sh
#test_run "test applicability_check element" $srcdir/test_applicability_check.sh
test_run "count function"  $srcdir/test_count_function.sh
test_exit

