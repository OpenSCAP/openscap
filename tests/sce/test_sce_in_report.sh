#!/bin/bash

# Author:
#	Jan Černý <jcerny@redhat.com>

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_sce_in_report {

    local ret_val=0
    local DEFFILE=${srcdir}/$1
    local RESFILE=$1.results
	local REPORTFILE=$1.report.html

    [ -f $RESFILE ] && rm $RESFILE

    $OSCAP xccdf eval --check-engine-results --results "$RESFILE" "$DEFFILE"
	$OSCAP xccdf generate report "$RESFILE" > "$REPORTFILE"
	list=`ls -al /`
	grep "$list" "$REPORTFILE"

}

test_init "test_sce_in_ds.log"
test_run "sce results in HTML report" test_sce_in_report test_sce_in_report.xml
test_exit
