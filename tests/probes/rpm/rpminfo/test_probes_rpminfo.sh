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
. $srcdir/rpminfo_common.sh

set -e -o pipefail

require "rpm" || exit 255
A_NAME=`rpm --qf "%{NAME}\n" -qa | sort | uniq -u | sed -n '1p'`
B_NAME=`rpm --qf "%{NAME}\n" -qa | sort | uniq -u | sed -n '2p'`

[ -n "$A_NAME" ] || exit 255

test_init

test_run "rpminfo probe test" test_probes_rpminfo $A_NAME $B_NAME

test_exit