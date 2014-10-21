#!/bin/bash

. ../../test_common.sh

test_init tests_probes_process88.log
test_run "Ensure that selinux_domain_label is collected" $srcdir/selinux_domain_label.sh
test_run "Ensure that tty number is translated into name" $srcdir/dev_to_tty.sh
test_run "Ensure loguid return unsighned int (cat /proc/ID/loginuid)" $srcdir/loginuid.sh
test_run "Ensure sessionid is correct" $srcdir/sessionid.sh
test_exit
