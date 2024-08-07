#!/usr/bin/env bash

. $builddir/tests/test_common.sh

function good-ok {
	$OSCAP oval validate ${srcdir}/oval-ok.xml
	ret=$?
	if [ $ret -eq 0 ]; then
		return 0
	fi
	return 1
}

function oval-no-xml {
	$OSCAP oval validate ${srcdir}/all.sh
	ret=$?
	if [ $ret -eq 1 ]; then
		return 0
	fi
	return 1
}

function oval-schema-fail {
	$OSCAP oval validate ${srcdir}/oval-schema-fail.xml
	ret=$?
	if [ $ret -eq 2 ]; then
		return 0
	fi
	return 1
}

function oval-schematron-fail {
	$OSCAP oval validate ${srcdir}/oval-schematron-fail.xml
	ret=$?
	if [ $ret -eq 2 ]; then
		return 0
	fi
	return 1
}

test_init test_validation.log
test_run "oval-ok" good-ok
test_run "oval-no-xml" oval-no-xml
test_run "oval-schema-fail" oval-schema-fail
test_run "oval-schematron-fail" oval-schematron-fail
test_exit

