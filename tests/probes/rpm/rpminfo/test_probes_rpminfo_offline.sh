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
#      Evgenii Kolesnikov, <ekolesni@redhat.com>

. $builddir/tests/test_common.sh
. $srcdir/rpminfo_common.sh

set -e -o pipefail


test_init

rpm_prepare_offline

test_run "rpminfo probe test (offline)" test_probes_rpminfo foo foobar

rpm_cleanup_offline

test_exit