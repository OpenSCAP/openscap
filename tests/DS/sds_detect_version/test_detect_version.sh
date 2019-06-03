#!/bin/bash

# Copyright 2019 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenSCAP Test Suite
#
# Authors:
#      Jan Černý <jcerny@redhat.com>

. $builddir/tests/test_common.sh

set -e -o pipefail

echo $srcdir

function test_oscap_info {
	version="$1"
	stdout="$(mktemp)"
	stderr="$(mktemp)"
	ds="$(mktemp)"
	cp $srcdir/scap-ds.xml $ds
	sed -i "s/X.X/${version}/g" $ds

	$OSCAP info $ds > $stdout 2> $stderr
	[ ! -s $stderr ]
	grep -q "Version: $version" $stdout
	rm $stdout
	rm $stderr
	rm $ds
}

SDS=$(find $top_srcdir/schemas/sds -maxdepth 1 -mindepth 1 -type d -printf '%f\n')

for sds_version in $SDS
do
    test_oscap_info $sds_version
done
