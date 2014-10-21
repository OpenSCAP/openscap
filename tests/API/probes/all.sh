#!/bin/bash

. ../../test_common.sh

test_init "test_api_probes.log"
test_run "fts test" $srcdir/fts.sh
test_run "probe api smoke test" ./test_api_probes_smoke
test_exit
