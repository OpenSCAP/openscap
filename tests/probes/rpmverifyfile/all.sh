#!/bin/bash

. ../../test_common.sh

test_init "test_probes_rpmverifyfile.log"
test_run "rpmverifyfile probe test with OVAL 5.11.1" $srcdir/test_probes_rpmverifyfile.sh
test_run "rpmverifyfile probe test with OVAL 5.11" $srcdir/test_probes_rpmverifyfile_older.sh
test_exit
