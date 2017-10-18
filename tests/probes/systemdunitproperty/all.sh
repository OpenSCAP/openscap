#!/bin/bash

. $builddir/tests/test_common.sh

test_init "test_probes_systemdunitproperty.log"
test_run "systemdunitproperty general functionality" $srcdir/test_probes_systemdunitproperty.sh
test_run "systemdunitproperty mount Wants - only on some systems" $srcdir/test_probes_systemdunitproperty_mount_wants.sh
test_exit
