#!/usr/bin/env bash

. $builddir/tests/test_common.sh

if [ -n "${CUSTOM_OSCAP+x}" ] ; then
    exit 255
fi

./test_fsdev_is_local_fs $srcdir/fake_mtab