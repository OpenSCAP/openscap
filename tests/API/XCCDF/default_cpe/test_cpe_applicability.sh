#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. $builddir/tests/test_common.sh

CPE_OVAL="cpe-oval.xml"
CPE_DICT="cpe-dict.xml"
XCCDF="xccdf.xml"
CPE="cpe:/o:example:applicable:1"
NA_CPE="cpe:/o:example:not_applicable:5"

function test_cpe_setup() {
	cp $srcdir/$CPE_OVAL $builddir/$CPE_OVAL
	cp $srcdir/$CPE_DICT $builddir/$CPE_DICT
	echo "Applicable CPE (set in '$CPE_DICT'): $CPE"
}

function test_cpe {
	local CPE=$1
	local EXPECTED_NA=$2
	local TMP_RESULTS=`mktemp`

	# Create XCCDF file with CPE based on RHEL/FEDORA version.
	rm -f $builddir/$XCCDF
	cp $srcdir/$XCCDF $builddir/$XCCDF
	sed -i "s|<<CPE>>|${CPE}|" $builddir/$XCCDF

	echo "Tested CPE: $CPE"
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

test_init "test_api_xccdf_cpe_applicability.log"
test_cpe_setup

test_run "test_api_xccdf_cpe_applicable" test_cpe \
	$CPE 0
test_run "test_api_xccdf_cpe_notapplicable" test_cpe \
	$NA_CPE 1

test_exit
