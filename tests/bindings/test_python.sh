#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

. ../test_common.sh

# Test Cases.

function test_python_import {
    python -c "import openscap_api"
}

# Testing.
test_init "test_python.log"

test_run "python_import" test_python_import

test_exit
