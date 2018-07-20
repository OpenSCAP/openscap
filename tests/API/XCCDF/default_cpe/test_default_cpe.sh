#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. $builddir/tests/test_common.sh

CPE_OVAL="cpe-oval.xml"
FEDORA_XCCDF="fedoraX-xccdf.xml"
FEDORA_CPE="cpe:/o:fedoraproject:fedora:"
RHEL_XCCDF="rhelX-xccdf.xml"
RHEL_CPE="cpe:/o:redhat:enterprise_linux:"
SYSTEM_CPE_FILE="/etc/system-release-cpe"

# System specific version variable is filled with system version number only
# if this test runs on the specific system. Otherwise it is left empty.
FEDORA_VERSION=$(grep "${FEDORA_CPE}" $SYSTEM_CPE_FILE \
	| sed "s|${FEDORA_CPE}\([0-9]\+\).*|\1|")
RHEL_VERSION=$(grep "${RHEL_CPE}" $SYSTEM_CPE_FILE \
	| sed "s|${RHEL_CPE}\([0-9]\+\).*|\1|")
# In case the version variable is empty we set some default values to also
# test notapplicable result.
[ -z "$FEDORA_VERSION" ] && FEDORA_VERSION=28
[ -z "$RHEL_VERSION" ] && RHEL_VERSION=7


function test_cpe_fedora_rhel {
	local VERSION=$1
	local INPUT_TEMPLATE=$2
	local CPE=$3
	local INPUT=$(echo "$INPUT_TEMPLATE" | sed "s|X|${VERSION}|")
	local TMP_RESULTS=`mktemp`
	local EXPECTED_NA=0
	local SYSTEM_CPE=$(cat $SYSTEM_CPE_FILE)

	# Create XCCDF file with CPE based on RHEL/FEDORA version.
	cat $srcdir/$INPUT_TEMPLATE > $builddir/$INPUT
	cp $srcdir/$CPE_OVAL $builddir/$CPE_OVAL
	sed -i "s|${CPE}X|${CPE}${VERSION}|" $builddir/$INPUT

	if ! echo "$SYSTEM_CPE" | grep -q "${CPE}${VERSION}"; then
		EXPECTED_NA=1
	fi

	echo "System CPE: ${SYSTEM_CPE}"
	echo "Tested CPE: ${CPE}${VERSION}"
	$OSCAP xccdf eval --results $TMP_RESULTS $builddir/$INPUT
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

test_run "test_api_xccdf_default_cpe_fedora" test_cpe_fedora_rhel \
	$FEDORA_VERSION $FEDORA_XCCDF $FEDORA_CPE
test_run "test_api_xccdf_default_cpe_rhel" test_cpe_fedora_rhel \
	$RHEL_VERSION $RHEL_XCCDF $RHEL_CPE

test_exit
