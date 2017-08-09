#!/usr/bin/env bash

# Copyright 2017 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -x

. ../../test_common.sh

name=$(basename $0 .sh)
export=$(mktemp -t ${name}.export.xml.XXXXXX)
results=$(mktemp -t ${name}.results.xml.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)

function test_api_cvrf_eval {
	./test_api_cvrf --eval $srcdir/$name.xml $results >$stdout 2>$stderr
	[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
	[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
	
	rm $results
}

function test_api_cvrf_export {
	local ret_val=0

	./test_api_cvrf --export-all $srcdir/$name.xml $export >$stdout 2>$stderr
	[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
	[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
	if [ $? -eq 0 ] && [ -f $export ]; then
	if ! $XMLDIFF $srcdir/$name.xml $export; then
		echo "Exported file differs from what is expected!"
		ret_val=1
	fi
	else
	echo "Cannot export!"
	ret_val=1
	fi
	rm $export
	return $ret_val
}

function test_api_cvrf_validate {
	local ret_val=0

	./test_api_cvrf --validate $srcdir/$name.xml >$stdout 2>$stderr
	[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
	[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
	if [ $? -eq 0 ]; then
	echo "Provided CVRF file is valid"
	ret_val=0
	else
	echo "Provided CVRF file does not adhere to specifications"
	ret_val=1
	fi
	return $ret_val
}

test_init "test_api_cvrf.log"
test_run "test_api_cvrf_export" test_api_cvrf_export
test_run "test_api_cvrf_eval" test_api_cvrf_eval
test_run "test_api_cvrf_validate" test_api_cvrf_validate
test_exit