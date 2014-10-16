#!/bin/bash

. ../../test_common.sh

test_init test_probes_sql57.log
test_run "Unsupported database engine" $srcdir/unsupported_engine.sh
test_exit
