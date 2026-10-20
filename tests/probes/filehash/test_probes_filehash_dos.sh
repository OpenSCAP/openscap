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

probecheck "filehash" || return 255
require "md5sum" || return 255
require "sha1sum" || return 255

# Block the scan indefinitely
rm -f /tmp/oscap-test-fifo
mkfifo /tmp/oscap-test-fifo

# Burn CPU forever
rm -f /tmp/oscap-test-symlink
ln -s /dev/zero /tmp/oscap-test-symlink

result="$(mktemp)"

$OSCAP oval eval --results "$result" "$srcdir/dos.oval.xml"
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:xxx:tst:1"][@result="error"]'
assert_exists 1 '/oval_results/results/system/tests/test[@test_id="oval:xxx:tst:2"][@result="error"]'

rm -f /tmp/oscap-test-fifo
rm -f /tmp/oscap-test-symlink
rm -f "$result"
