#!/bin/bash

. ../../test_common.sh

test_init "test_api_probes.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "fts test" $srcdir/fts.sh
    test_run "probe api smoke test" ./test_api_probes_smoke
    test_run "fsdev is_local_fs unit test" ./test_fsdev_is_local_fs
fi

test_exit
