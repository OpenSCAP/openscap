#!/bin/bash

. "$builddir/tests/test_common.sh"

test_init test_probes_sysctl.log
test_run "test sysctl probe" $srcdir/test_sysctl_probe.sh
test_run "test sysctl probe that collects everything" $srcdir/test_sysctl_probe_all.sh
test_exit
