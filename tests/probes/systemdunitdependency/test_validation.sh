#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.
#
# Authors:
#   Jan Černý <jcerny@redhat.com>

. "$builddir/tests/test_common.sh"
set -e

DF="${srcdir}/test_probes_systemdunitdependency.xml"

$OSCAP oval validate --schematron $DF

