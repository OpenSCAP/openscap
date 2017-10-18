#!/bin/bash

# Test to check usage of SCE in DS
#
# Author:
#	Zbynek Moravec <zmoravec@redhat.com>

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_sce_in_ds {

	xccdf_file=${srcdir}/$1
	stderr=$(mktemp)
	result=$(mktemp)

	$OSCAP xccdf eval --results "$result" "$xccdf_file" 2> $stderr

	assert_exists 1 "//result[text()='pass']"

	[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
	[ -f $result ]; [ -s $result ]; rm $result
}

# Testing.
test_init "test_sce_in_ds.log"

test_run "SCE in DS" test_sce_in_ds test_sce_in_ds.xml

test_exit
