#!/bin/bash

# Copyright 2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. ../test_common.sh

function test_cpe() {
    $OSCAP oval validate --schematron ${top_srcdir}/cpe/openscap-cpe-oval.xml
    $OSCAP cpe validate ${top_srcdir}/cpe/openscap-cpe-dict.xml
}

test_init "test_cpe.log"

test_run "Check validity of cpe/openscap-cpe-oval.xml" test_cpe
test_exit
