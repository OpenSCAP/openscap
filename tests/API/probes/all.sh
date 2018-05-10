#!/bin/bash

. "$builddir/tests/test_common.sh"

test_init "test_api_probes.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "fts test" $srcdir/fts.sh
    test_run "probe api smoke test" ./test_api_probes_smoke
fi

test_exit
