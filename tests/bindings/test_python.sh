#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

. $builddir/tests/test_common.sh

# Test Cases.

function test_python_import {
    bash $builddir/run python -c "import openscap_api"
}

# Testing.
test_init

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "python_import" test_python_import
fi

test_exit
