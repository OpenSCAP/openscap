#!/bin/bash

. $srcdir/../../test_common.sh

test_init tests_probes_process88.log
test_run "Ensure that selinux_domain_label is collected" $srcdir/selinux_domain_label.sh
test_exit
