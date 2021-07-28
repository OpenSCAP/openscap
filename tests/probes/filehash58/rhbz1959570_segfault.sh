#!/usr/bin/env bash

# Copyright 2021 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenSCAP Probes Test Suite.
#
# Authors:
#      Jan Černý, <jcerny@redhat.com>

set -e -o pipefail
. $builddir/tests/test_common.sh

# Test Cases

stderr="$(mktemp)"
$OSCAP oval eval --id oval:x:def:1 "$srcdir/rhbz1959570_segfault_reproducer.xml" 2> "$stderr"
[ ! -s "$stderr" ]
rm "$stderr"
