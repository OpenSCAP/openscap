#!/usr/bin/env bash

# Author:
#	Zbynek Moravec <zmoravec@redhat.com>

# We were getting parse error when oscap_source struct tried to read .sh
# file as xml. Main purpose of the test is check stderr of output
# More info: https://github.com/OpenSCAP/openscap/issues/230

set -e -o pipefail

. ../test_common.sh

# In this test oscap should try to load corrupted/invalid (XML) file and get
# error
function test_load_with_error {
	xccdf_file=${srcdir}/$1
	stderr=$(mktemp)
	result=$(mktemp)

	$OSCAP ds sds-compose "$xccdf_file" "$result" 2> $stderr

	[ -f $stderr ]; [ -s $stderr ]; rm $stderr		# we WANT stderr
	[ -f $result ]; [ -s $result ]; rm $result
}


# In this test oscap should try to load (SCE) file which is not XML.
# We don't want to see some error on stderr
function test_load_without_error {
	xccdf_file=${srcdir}/$1
	stderr=$(mktemp)
	result=$(mktemp)

	$OSCAP ds sds-compose "$xccdf_file" "$result" 2> $stderr

	[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr	# we do NOT want stderr
	[ -f $result ]; [ -s $result ]; rm $result
}

# Testing.
test_init "test_sce_parse_errors.log"

test_run "Load script WITHOUT error on stderr"	test_load_without_error	./test_sce_parse_errors_load_script.xccdf.xml
test_run "Load script WITH error on stderr"		test_load_with_error	./test_sce_parse_errors_load_corrupted_xml.xccdf.xml

test_exit

