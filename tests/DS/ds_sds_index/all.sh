#!/bin/bash

. $srcdir/../../test_common.sh

test_init ds_sds_index.log
test_run "ds_sds_index" ./test_ds_sds_index
test_exit
