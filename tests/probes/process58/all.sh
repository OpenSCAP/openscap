#!/bin/bash

. ../../test_common.sh

test_init tests_probes_process88.log
test_run "Ensure that selinux_domain_label is collected" $srcdir/selinux_domain_label.sh
test_run "Ensure that tty number is translated into name" $srcdir/dev_to_tty.sh
test_run "Ensure loguid returns unsigned int (cat /proc/ID/loginuid)" $srcdir/loginuid.sh
test_run "Ensure sessionid is correct" $srcdir/sessionid.sh
test_run "Ensure capabilities with OVAL 5.11" $srcdir/capability.sh
test_run "Ensure that command_line is collected" $srcdir/command_line.sh
test_exit
