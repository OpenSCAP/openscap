#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

. ${srcdir}/../test_common.sh

# Test Cases.

function test_sds {

    local ret_val=0;

    local XCCDF_FILE="${srcdir}/$1"
    local XCCDF_DIR="`dirname ${XCCDF_FILE}`"
    local DS_TARGET_DIR="`mktemp -d`"
    local DS_FILE="$DS_TARGET_DIR/sds.xml"

    local OSCAP_DIR=`cd ../../utils/.libs; pwd`

    pushd "$XCCDF_DIR"

    $OSCAP_DIR/oscap ds sds-compose "`basename $XCCDF_FILE`" "$DS_FILE"

    popd

    pushd "$DS_TARGET_DIR"

    $OSCAP_DIR/oscap ds sds-split "`basename $DS_FILE`" "$DS_TARGET_DIR"

    rm sds.xml

    popd

    DIFFERENCE=$(diff "$XCCDF_DIR" "$DS_TARGET_DIR")

    if [ $? -ne 0 ]; then
        echo "The files are different after going through source data stream! diff follows:"
        echo "$DIFFERENCE"
        echo

        ret_val=1
    fi

    rm -r "$DS_TARGET_DIR"

    return "$ret_val"
}

function test_eval {

    local OSCAP_DIR=`cd ../../utils/.libs; pwd`

    $OSCAP_DIR/oscap xccdf eval "$1"
    return $?
}
function test_rds
{
    local ret_val=0;

    local SDS_FILE="${srcdir}/$1"
    local XCCDF_RESULT_FILE="${srcdir}/$2"
    local OVAL_RESULT_FILE="${srcdir}/$3"
    local DS_TARGET_DIR="`mktemp -d`"
    local DS_FILE="$DS_TARGET_DIR/rds.xml"

    local OSCAP_DIR=`cd ../../utils/.libs; pwd`

    $OSCAP_DIR/oscap ds rds-create "$SDS_FILE" "$DS_FILE" "$XCCDF_RESULT_FILE" "$OVAL_RESULT_FILE"

    if [ $? -ne 0 ]; then
        ret_val=1
    fi

    #pushd "$DS_TARGET_DIR"
    #$OSCAP_DIR/oscap ds sds_split "`basename $DS_FILE`" "$DS_TARGET_DIR"
    #rm sds.xml
    #popd

    rm -r "$DS_TARGET_DIR"

    return "$ret_val"
}

# Testing.
test_init "test_ds.log"

test_run "sds_simple_xccdf" test_sds sds_simple/scap-fedora14-xccdf.xml
test_run "sds_multiple_oval" test_sds sds_multiple_oval/multiple-oval-xccdf.xml

test_run "eval_simple" test_eval eval_simple/sds.xml

test_run "rds_simple" test_rds rds_simple/sds.xml rds_simple/results-xccdf.xml rds_simple/results-oval.xml
test_run "rds_testresult" test_rds rds_testresult/sds.xml rds_testresult/results-xccdf.xml rds_testresult/results-oval.xml

test_exit

