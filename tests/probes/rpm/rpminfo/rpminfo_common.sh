#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.
#
# Created on: Nov 30, 2009
#
# Authors:
#      Peter Vrabec, <pvrabec@redhat.com>
#      David Niemoller
#      Ondrej Moris, <omoris@redhat.com>

. $builddir/tests/test_common.sh
. $srcdir/../rpm_common.sh

set -e -o pipefail


function test_probes_rpminfo {
    probecheck "rpminfo" || return 255
    require "rpm" || return 255

    local ret_val=0;
    local DF="test_probes_rpminfo.xml"
    local RF="$(mktemp results.XXXXXXX.xml)"

    rm -f $RF

    RPM_A_NAME=$1
    RPM_B_NAME=$2
    RPM_A_ARCH=`rpm_query $RPM_A_NAME ARCH`
    RPM_A_VERSION=`rpm_query $RPM_A_NAME VERSION`
    RPM_A_RELEASE=`rpm_query $RPM_A_NAME RELEASE`
    RPM_B_ARCH=`rpm_query $RPM_B_NAME ARCH`
    RPM_B_VERSION=`rpm_query $RPM_B_NAME VERSION`
    RPM_B_RELEASE=`rpm_query $RPM_B_NAME RELEASE`

    bash ${srcdir}/rpminfo.xml.sh $RPM_A_NAME $RPM_A_ARCH $RPM_A_VERSION $RPM_A_RELEASE $RPM_B_NAME $RPM_B_ARCH $RPM_B_VERSION $RPM_B_RELEASE > $DF
    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 200
        ret_val=$?
    else
        ret_val=1
    fi

    rm -f $RF $DF

    return $ret_val
}
