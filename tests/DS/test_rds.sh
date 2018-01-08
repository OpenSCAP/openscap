#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

set -e -o pipefail

. $builddir/tests/test_common.sh

# Test Cases.

function assert_correct_xlinks()
{
	local DS=$1
	local stderr=$(mktemp)
	$OSCAP info $DS 2> $stderr
	diff $stderr /dev/null
	rm $stderr

	# First of all make sure that there is at least one ds:component-ref.
	[ "$($XPATH $DS 'count(//*[local-name()="component-ref"])')" != "0" ]
	# We want to catch cases when this element has different namespace.
	local ns=$($XPATH $DS 'name(//*[local-name()="component-ref"][1])' | sed 's/:.*$/:/')
	[ "$ns" != "component-ref" ] || ns=""
	# Ensure there is at least some xlink.
	[ "`$XPATH $DS \"count(//${ns}component-ref/@xlink:href)\"`" != "0" ]
	# This asserts that there is none component-ref/xlink:href broken.
	# Previously, we have seen datastreams with broken xlinks (see trac#286).
	[ "`$XPATH $DS  \"count(//${ns}component-ref[substring(@xlink:href, 2, 10000) != (//${ns}component/@${ns}id | //${ns}extended-component/@${ns}id)])\"`" == "0" ]
}

function test_rds
{
    local ret_val=0;

    local SDS_FILE="${srcdir}/$1"
    local XCCDF_RESULT_FILE="${srcdir}/$2"
    local OVAL_RESULT_FILE="${srcdir}/$3"
    local DS_TARGET_DIR="`mktemp -d`"
    local DS_FILE="$DS_TARGET_DIR/rds.xml"

    $OSCAP ds rds-create "$SDS_FILE" "$DS_FILE" "$XCCDF_RESULT_FILE" "$OVAL_RESULT_FILE"

    if [ $? -ne 0 ]; then
        ret_val=1
    fi

    assert_correct_xlinks $DS_FILE

    #pushd "$DS_TARGET_DIR"
    #$OSCAP ds sds_split "`basename $DS_FILE`" "$DS_TARGET_DIR"
    #rm sds.xml
    #popd

    rm -r "$DS_TARGET_DIR"

    return "$ret_val"
}

function test_rds_index
{
    local ret_val=0;

    local RDS_FILE="${srcdir}/$1"
    local ASSETS="$2"
    local REPORTS="$3"
    local REQUESTS="$4"

    INDEX=$($OSCAP info "$RDS_FILE")

    for asset in "$ASSETS"; do
        if ! echo $INDEX | grep --quiet "$asset"; then
            ret_val=1
            echo "Asset $asset expected in index"
        fi
    done

    for report in "$REPORTS"; do
        if ! echo $INDEX | grep --quiet "$report"; then
            ret_val=1
            echo "Report $report expected in index"
        fi
    done

    for requests in "$REQUESTS"; do
        if ! echo $INDEX | grep --quiet "$request"; then
            ret_val=1
            echo "Report request $request expected in index"
        fi
    done

    return "$ret_val"
}

function test_rds_split {

    local DIR="${srcdir}/$1"
    local SDS_FILE="$2"
    local REPORT_FILE="$3"
    local SKIP_DIFF="$4"
    local DS_TARGET_DIR="`mktemp -d`"
    local DS_FILE="$DS_TARGET_DIR/arf.xml"

    pushd "$DIR"

    $OSCAP ds rds-create "$SDS_FILE" "$DS_FILE" "$REPORT_FILE"

    assert_correct_xlinks $DS_FILE
    popd

    pushd "$DS_TARGET_DIR"

    $OSCAP ds rds-split "`basename $DS_FILE`" "$DS_TARGET_DIR"

    rm "$DS_FILE"
    popd

    if [ "$SKIP_DIFF" != "1" ]; then
        if ! diff --exclude "oscap_debug.log.*" "$DIR" "$DS_TARGET_DIR"; then
            echo "The files are different after going through result data stream!"
            echo
            return 1
        fi
    fi

    rm -r "$DS_TARGET_DIR"
    return 0
}

# Testing.
test_init "test_rds.log"

test_run "rds_simple" test_rds rds_simple/sds.xml rds_simple/results-xccdf.xml rds_simple/results-oval.xml
test_run "rds_testresult" test_rds rds_testresult/sds.xml rds_testresult/results-xccdf.xml rds_testresult/results-oval.xml
test_run "rds_index_simple" test_rds_index rds_index_simple/arf.xml "asset0 asset1" "report0" "collection0"
test_run "rds_split_simple" test_rds_split rds_split_simple report-request.xml report.xml 0

test_exit

