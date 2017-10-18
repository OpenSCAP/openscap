#!/bin/bash

# Test to check there are output streams separated
#
# Author:
#   Marek Haicman <mhaicman@redhat.com>

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_sce_stdout_stderr {

    local xccdf_file=${srcdir}/$1
    local stderr=$(mktemp)
    local result=$(mktemp)

    $OSCAP xccdf eval --results "$result" "$xccdf_file" 2> $stderr
    echo "===== result ====="
    cat $result

    grep -q '<check-import import-name="stdout">test_out' $result && \
    grep -q '<check-import import-name="stderr">test_err' $result
}

# Testing.
test_init "test_sce_stdout_stderr.log"

test_run "SCE stdout and stderr" test_sce_stdout_stderr test_sce_stdout_stderr.xccdf.xml

test_exit
