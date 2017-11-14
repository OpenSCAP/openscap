#!/usr/bin/env bash

# Author:
#	Martin Preisler <mpreisle@redhat.com>

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_sce {

    local ret_val=0;
    local DEFFILE=${srcdir}/$1
    local RESFILE=$1.results

    [ -f $RESFILE ] && rm $RESFILE

    $OSCAP xccdf eval --results "$RESFILE" --profile "default" "$DEFFILE"

    LINES=`grep \<result\> "$RESFILE"`

    # calculate return code
    echo "$LINES" | grep -q -v "pass" || ret_val=$?
    [ $ret_val -eq 1 ]
}

test_init "test_sce.log"
test_run "sce" test_sce sce_xccdf.xml
test_exit
