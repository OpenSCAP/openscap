#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. $builddir/tests/test_common.sh

CPE_OVAL="cpe-oval.xml"
CPE_DICT="cpe-dict.xml"
XCCDF="xccdf.xml"
SYSTEM_CPE_FILE="/etc/system-release-cpe"
SYSTEM_CPE=$(cat $SYSTEM_CPE_FILE)
NA_CPE="cpe:/o:example:not_applicable:5"

function test_cpe_setup() {
	local CPE=$1
	cp $srcdir/$CPE_OVAL $builddir/$CPE_OVAL
	cp $srcdir/$CPE_DICT $builddir/$CPE_DICT
	sed -i "s|<<CPE>>|${CPE}|" $builddir/$CPE_DICT
}

function test_cpe {
	local CPE=$1
	local EXPECTED_NA=$2
	local TMP_RESULTS=`mktemp`

	# Create XCCDF file with CPE based on RHEL/FEDORA version.
	cp $srcdir/$XCCDF $builddir/$XCCDF
	sed -i "s|<<CPE>>|${CPE}|" $builddir/$XCCDF

	echo "System CPE: ${SYSTEM_CPE}"
	echo "Tested CPE: ${CPE}"
	$OSCAP xccdf eval --cpe $builddir/$CPE_DICT --results $TMP_RESULTS $builddir/$XCCDF
	if [ $? -ne 0 ]; then
		return 1
	fi

	local NOTAPPLICABLE_COUNT=$($XPATH $TMP_RESULTS \
		'count(//result[text()="notapplicable"])' 2>/dev/null)
	rm -f $TMP_RESULTS

	if [ "$NOTAPPLICABLE_COUNT" == "$EXPECTED_NA" ]; then
		return 0
	fi
	return 1
}

# Testing.

test_init "test_api_xccdf_default_cpe.log"
test_cpe_setup $SYSTEM_CPE

test_run "test_api_xccdf_default_cpe" test_cpe \
	$SYSTEM_CPE 0
test_run "test_api_xccdf_notapplicable_cpe" test_cpe \
	$NA_CPE 1

test_exit
