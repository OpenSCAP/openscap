#!/bin/bash

. "$builddir/tests/test_common.sh"

test_init "test_probes_systemdunitdependency.log"
test_run "systemdunitdependency general functionality" $srcdir/test_probes_systemdunitdependency.sh
test_run "OVAL 5.11 validation" $srcdir/test_validation.sh
test_exit
