#!/bin/bash

. $builddir/tests/test_common.sh

test_init "test_probes_symlink.log"
test_run "symlink probe general functionality" $srcdir/test_probes_symlink.sh
test_exit
