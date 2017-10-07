#!/bin/bash

set -e -o pipefail

. ../../test_common.sh

test_init ds_sds_index.log

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "ds_sds_index" ./test_ds_sds_index $srcdir/sds.xml
    test_run "ds_sds_index_multiple" ./test_ds_sds_index_multiple $srcdir/sds_multiple.xml
    test_run "ds_sds_index_invalid" ./test_ds_sds_index_invalid $srcdir/sds_invalid.xml
fi

test_exit
