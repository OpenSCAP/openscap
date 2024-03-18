#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_partition {
    probecheck "partition" || return 255
    require_non_chroot || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_partition.xml"
    local result="test_probes_partition.results.xml"
    echo "result file: $result"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $result ] && rm -f $result

    $OSCAP oval eval --results $result $DF 2>$stderr

    if [ -f $result ]; then
        verify_results "def" $DF $result 3 && verify_results "tst" $DF $result 3
        ret_val=$?
    else
        ret_val=1
    fi

    CO='/oval_results/results/system/oval_system_characteristics/collected_objects'
    assert_exists 1 $CO'/object[@id="oval:1:obj:1" and @flag="complete"]'
    assert_exists 1 $CO'/object[@id="oval:1:obj:2" and @flag="complete"]'
    assert_exists 1 $CO'/object[@id="oval:1:obj:3" and @flag="does not exist"]'
    SD='/oval_results/results/system/oval_system_characteristics/system_data'
    assert_exists 1 $SD'/lin-sys:partition_item'
    assert_exists 1 $SD'/lin-sys:partition_item/lin-sys:mount_point[text()="/proc"]'

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"
    rm $stderr

    return $ret_val
}

test_run "Probe partition simple test (/proc mount point)" test_probes_partition
