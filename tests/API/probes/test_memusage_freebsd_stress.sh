#!/usr/bin/env bash

. $builddir/tests/test_common.sh

if [[ "$(uname)" != "FreeBSD" ]] ; then
	exit 255
fi

./test_memusage_freebsd_stress
