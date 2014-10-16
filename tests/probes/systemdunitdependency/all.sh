#!/bin/bash

. ../../test_common.sh

test_init "test_probes_systemdunitdependency.log"
test_run "systemdunitdependency general functionality" $srcdir/test_probes_systemdunitdependency.sh
test_exit
