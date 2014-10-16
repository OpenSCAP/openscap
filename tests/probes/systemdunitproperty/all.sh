#!/bin/bash

. ../../test_common.sh

test_init "test_probes_systemdunitproperty.log"
test_run "systemdunitproperty general functionality" $srcdir/test_probes_systemdunitproperty.sh
test_exit
