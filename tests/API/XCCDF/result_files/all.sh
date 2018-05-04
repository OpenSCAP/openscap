#!/usr/bin/env bash

# Copyright 2014 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. "$builddir/tests/test_common.sh"

function test_api_xccdf_result_files_cpe_conflict {
    local ABS_SRC_DIR=`cd $srcdir; pwd`

    local INPUT=$1
    local CPE=$2
    local OVAL=$3

    local FIRST_OVAL_RESULT=$4
    local SECOND_OVAL_RESULT=$5

    local TMP_RESULT_DIR=`mktemp -d`
    cd $TMP_RESULT_DIR

    cp $ABS_SRC_DIR/$INPUT .
    cp $ABS_SRC_DIR/$CPE .
    cp $ABS_SRC_DIR/$OVAL .

    $OSCAP xccdf eval --cpe $CPE --oval-results $INPUT

    echo "First OVAL result:"
    cat $FIRST_OVAL_RESULT
    echo "Second OVAL result:"
    cat $SECOND_OVAL_RESULT

    rm -rf "$TMP_RESULT_DIR"

    return 0
}

# Testing.

test_init "test_api_xccdf_result_files.log"

test_run "test_api_xccdf_result_files_cpe_conflict" test_api_xccdf_result_files_cpe_conflict simple-xccdf.xml cpe-dict.xml cpe-oval.xml cpe-oval.xml.result.xml cpe-oval.xml.result2.xml

test_exit
