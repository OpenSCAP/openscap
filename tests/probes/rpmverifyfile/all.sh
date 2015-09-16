#!/bin/bash

. ../../test_common.sh

test_init "test_probes_rpmverifyfile.log"
test_run "rpmverifyfile probe test" $srcdir/test_probes_rpmverifyfile.sh
test_exit
