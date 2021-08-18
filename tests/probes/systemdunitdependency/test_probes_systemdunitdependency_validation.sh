#!/usr/bin/env bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.
#
# Authors:
#   Jan Černý <jcerny@redhat.com>

. $builddir/tests/test_common.sh

set -e

function oval_validation {
    DF="test_probes_systemdunitdependency_validate.xml"
    ${srcdir}/test_probes_systemdunitdependency.xml.sh "true" > $DF
    $OSCAP oval validate --schematron $DF
}

test_run "OVAL 5.11 validation (systemdunitdependency)" oval_validation


