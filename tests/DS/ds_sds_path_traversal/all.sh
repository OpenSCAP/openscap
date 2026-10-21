#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

test_init ds_sds_path_traversal.log

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "path_traversal" ./test_ds_sds_path_traversal $srcdir
fi

test_exit
