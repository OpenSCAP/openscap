#!/usr/bin/env bash

# Copyright 2011 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. ${srcdir}/../../test_common.sh

# Test Cases.

function test_probes_fileextendedattribute {
    return 255 # TODO: implement xattr support check

    if [ ! -x ${OVAL_PROBE_DIR}/probe_fileextendedattribute ]; then
        echo -e "Probe file does not exist!\n"
        return 255; # Test is not applicable.
    fi

    local ret_val=0;
    local DEFFILE="$srcdir/test_probes_fileextendedattribute.xml"
    local RESFILE="results.xml"

    [ -f $RESFILE ] && rm -f $RESFILE
    
    touch /tmp/xattr_with_val
    setfattr -n user.fooattr -v foo /tmp/xattr_with_val

    touch /tmp/xattr_without_val
    setfattr -n user.fooattr /tmp/xattr_without_val

    touch /tmp/xattr_noattr

    ../../../utils/.libs/oscap oval eval --results $RESFILE $DEFFILE

    if [ -f $RESFILE ]; then

        COUNT=1; ID=1

        while [ $ID -le $COUNT ]; do

            DEF_DEF=`cat "$DEFFILE" | grep "id=\"oval:1:def:\${ID}\""`
            DEF_RES=`cat "$RESFILE" | grep "definition_id=\"oval:1:def:${ID}\""`

            if (echo $DEF_RES | grep "result=\"true\"" >/dev/null); then
                RES="TRUE"
            elif (echo $DEF_RES | grep "result=\"false\"" >/dev/null); then
                RES="FALSE"
            else
                RES="ERROR"
            fi

            if (echo $DEF_DEF | grep "comment=\"true\"" >/dev/null); then
                CMT="TRUE"
            elif (echo $DEF_DEF | grep "comment=\"false\"" >/dev/null); then
                CMT="FALSE"
            else
                CMT="ERROR"
            fi

            if [ ! $RES = $CMT ]; then
                echo "Result of oval:1:def:${ID} should be ${CMT}!"
                ret_val=$[$ret_val + 1]
            fi

            ID=$[$ID+1]
        done

        COUNT=3; ID=1
        while [ $ID -le $COUNT ]; do

            TEST_DEF=`cat "$DEFFILE" | grep "id=\"oval:1:tst:${ID}\""`
            TEST_RES=`cat "$RESFILE" | grep "test_id=\"oval:1:tst:${ID}\""`

            if (echo $TEST_RES | grep "result=\"true\"" >/dev/null); then
                RES="TRUE"
            elif (echo $TEST_RES | grep "result=\"false\"" >/dev/null); then
                RES="FALSE"
            else
                RES="ERROR"
            fi

            if (echo $TEST_DEF | grep "comment=\"true\"" >/dev/null); then
                CMT="TRUE"
            elif (echo $TEST_DEF | grep "comment=\"false\"" >/dev/null); then
                CMT="FALSE"
            else
                CMT="ERROR"
            fi

            if [ ! $RES = $CMT ]; then
                echo "Result of oval:1:tst:${ID} should be ${CMT}!"
                ret_val=$[$ret_val + 1]
            fi

            ID=$[$ID+1]
        done
    else
        ret_val=1
    fi

    return $ret_val
}

# Testing.
test_init "test_probes_fileattribute.log"
test_run "test_probes_fileattribute" test_probes_fileextendedattribute
test_exit
