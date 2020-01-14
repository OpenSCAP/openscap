#!/usr/bin/env bash

# Copyright 2015-2019 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenSCAP Probes Test Suite.
# Tests the rpmverifypackage probe.
#
# Authors:
#      Jan Černý <jcerny@redhat.com>
#      Evgenii Kolesnikov <ekolesni@redhat.com>

. $builddir/tests/test_common.sh
. $srcdir/rpmverifypackage_common.sh

set -e -o pipefail
set -x

[ -f /etc/os-release ] && . /etc/os-release


function test_probes_rpmverifypackage_epoch {
    RPM_package_1=$(rpm -qa --qf "%{NAME}\t%{EPOCH}\n" | grep -v "(none)" | sort -u | head -1 | cut -f 1) || ret=$?
    test_probes_rpmverifypackage $RPM_package_1
}

function test_probes_rpmverifypackage_noepoch {
    RPM_package_2=$(rpm -qa --qf "%{NAME}\t%{EPOCH}\n" | grep "(none)" | sort -u | head -1 | cut -f 1) || ret=$?
    test_probes_rpmverifypackage $RPM_package_2
}

function test_probes_rpmverifypackage_rpm {
    test_probes_rpmverifypackage rpm
}


test_init

if [[ $ID != *"sle"* && $ID != *"suse"* ]]; then
    test_run "rpmverifypackage probe test with epoch" test_probes_rpmverifypackage_epoch
else
    test_run "rpmverifypackage probe test" test_probes_rpmverifypackage_noepoch
fi

test_exit
