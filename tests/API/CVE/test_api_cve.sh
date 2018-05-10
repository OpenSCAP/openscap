#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CvE Module Test Suite.
#
# Authors:
#      Maros Barabas <mbarabas@redhat.com>
#      Ondrej Moris <omoris@redhat.com>

. "$builddir/tests/test_common.sh"

function test_api_cve_cvss {
     ./test_api_cve --test-cvss $srcdir/nvdcve-2.0-recent.xml
}

function test_api_cve_export {
    local ret_val=0

    ./test_api_cve --export-all $srcdir/nvdcve-2.0-recent.xml nvdcve-2.0-recent.out.xml
    if [ $? -eq 0 ] && [ -f nvdcve-2.0-recent.out.xml ]; then
 	if ! $XMLDIFF $srcdir/nvdcve-2.0-recent.xml nvdcve-2.0-recent.out.xml; then
	    echo "Exported file differs from what is expected!"
	    ret_val=1
	fi
    else
 	echo "Cannot export!"
	ret_val=1
    fi
    return $ret_val
}

test_init "test_api_cve.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_api_cve_cvss" test_api_cve_cvss
    test_run "test_api_cve_export" test_api_cve_export
fi

test_exit

