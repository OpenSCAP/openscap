#!/bin/bash

set -e -o pipefail

. $builddir/tests/test_common.sh

test_init ds_sds_index.log

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "ds_sds_index" ./test_ds_sds_index $srcdir/sds.xml
    test_run "ds_sds_index_multiple" ./test_ds_sds_index_multiple $srcdir/sds_multiple.xml
fi

test_exit
