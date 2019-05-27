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

function test_oscap_info {
	version="$1"
	stdout="$(mktemp)"
	stderr="$(mktemp)"
	$OSCAP info $srcdir/scap-$version-ds.xml > $stdout 2> $stderr
	[ ! -s $stderr ]
	grep -q "Version: $version" $stdout
	rm $stdout
	rm $stderr
}

test_oscap_info "1.2"
test_oscap_info "1.3"
