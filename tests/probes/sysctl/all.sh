#!/bin/bash

. $srcdir/../../test_common.sh

test_init test_probes_sysctl.log
test_run "test sysctl probe" $srcdir/test_sysctl_probe.sh
test_exit
