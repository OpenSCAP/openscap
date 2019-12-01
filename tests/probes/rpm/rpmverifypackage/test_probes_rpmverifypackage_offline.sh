#!/usr/bin/env bash

# Copyright 2019 Red Hat Inc., Durham, North Carolina.
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
. $srcdir/../rpm_common.sh

set -e -o pipefail
set -x


test_init

rpm_prepare_offline

test_run "rpmverifypackage probe test" test_probes_rpmverifypackage "foobar"

rpm_cleanup_offline

test_exit