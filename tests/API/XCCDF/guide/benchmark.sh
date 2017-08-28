#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. ../../../test_common.sh


function test_generate_guide_benchmark {
    local INPUT=$srcdir/$1
    local BENCHMARK=$2
    local EXPECTED_CONTENT=$3

    local GENERATED_CONTENT=$($OSCAP xccdf generate guide --benchmark-id $BENCHMARK "$INPUT")
    if [ "$?" != "0" ]; then
        return 1
    fi

    echo "$GENERATED_CONTENT" | grep "$EXPECTED_CONTENT"
    if [ "$?" == "0" ]; then
        return 0
    fi

    echo "Generated content does not contain '$EXPECTED_CONTENT'!"
    echo "Generated content:"
    echo "$GENERATED_CONTENT"

    return 1
}

# Testing.

test_init "test_api_xccdf_guide_benchmark.log"

test_run "test_api_xccdf_guide_benchmark_id1" test_generate_guide_benchmark benchmark_id_test-ds.xml xccdf_org.ssgproject.content_benchmark_1 Benchmark1
test_run "test_api_xccdf_guide_benchmark_id2" test_generate_guide_benchmark benchmark_id_test-ds.xml xccdf_org.ssgproject.content_benchmark_2 Benchmark2

test_exit
