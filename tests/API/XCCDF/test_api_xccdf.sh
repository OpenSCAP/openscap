#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap XCCDF Module Test Suite.
#
# Authors:
#      Peter Vrabec <pvrabec@redhat.com>

set -e -o pipefail

. $srcdir/../../test_common.sh

# Test cases.

function test_api_xccdf_export {
	local INPUT=$srcdir/$1
	local OUTPUT=$1.out

	./test_api_xccdf --export $srcdir/$INPUT $OUTPUT

	# Workaround libxml2 on rhel5 which exports namespace declarations in different order.
	sed -i 's|<cpe-list xmlns="http://cpe.mitre.org/XMLSchema/cpe/1.0" xmlns:meta="http://scap.nist.gov/schema/cpe-dictionary-metadata/0.2" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"|<cpe-list xmlns:meta="http://scap.nist.gov/schema/cpe-dictionary-metadata/0.2" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns="http://cpe.mitre.org/XMLSchema/cpe/1.0"|' $OUTPUT

	if [ $? -eq 0 ]; then
		cmp $srcdir/$INPUT $OUTPUT
		if [ $? -ne 0 ]; then
			echo "Exported file differs from what is expected!"
			return 1
		fi
	else
		echo "Cannot export xccdf content!"
		return 1
	fi

	return 0
}

function test_api_xccdf_validate {
	local INPUT=$1
	local VER=$2

	./test_api_xccdf --validate $VER $srcdir/$INPUT
	return $([ $? -eq 0 ])
}

# Testing.

test_init "test_api_xccdf.log"

test_run "text_api_xccdf-1.1_export" test_api_xccdf_export scap-rhel5-xccdf11.xml
test_run "text_api_xccdf-1.1_validate" test_api_xccdf_validate scap-rhel5-xccdf11.xml "1.1"
test_run "text_api_xccdf-1.2_export" test_api_xccdf_export scap-rhel5-xccdf12.xml
test_run "text_api_xccdf-1.2_validate" test_api_xccdf_validate scap-rhel5-xccdf12.xml "1.2"

test_exit
