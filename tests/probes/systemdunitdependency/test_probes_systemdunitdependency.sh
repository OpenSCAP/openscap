#!/usr/bin/env bash

# Copyright 2014 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.

. ${srcdir}/../../test_common.sh

function test_probes_systemdunitdependency {

    probecheck "systemdunitdependency" || return 255

    # TODO
    return 0
}

test_probes_systemdunitdependency
