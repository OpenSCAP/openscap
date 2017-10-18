#!/bin/bash

. $builddir/tests/test_common.sh

test_init test_probes_rpmverify.log
test_run "not equals operation" $srcdir/test_not_equals_operation.sh
test_exit
