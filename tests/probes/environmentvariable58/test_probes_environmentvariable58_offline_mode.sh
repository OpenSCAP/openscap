#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_environmentvariable58_offline_mode {
    probecheck "environmentvariable58" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_environmentvariable58_offline_mode.sh.xml"
    local RF="test_probes_environmentvariable58_offline_mode.sh.results.xml"
    echo "result file: $RF"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_environmentvariable58.XXXXXX")
    mkdir -p "$tmpdir/proc/1"

    set_chroot_offline_test_mode "$tmpdir"

    OSCAP_CONTAINER_VARS=$'CONTAINER_VARIABLE=/some/value/for/container\nOTHER_CONTAINER_VAR=other' $OSCAP oval eval --results $RF $DF 2> $stderr

    unset_chroot_offline_test_mode

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"

    rm $stderr

    rm -rf ${tmpdir}

    return $ret_val
}

function test_probes_environmentvariable58_offline_mode_no_procfs {
    probecheck "environmentvariable58" || return 255

    local ret_val=0;
    local definition="${srcdir}/test_probes_environmentvariable58_offline_mode.sh.xml"
    local result="test_probes_environmentvariable58_offline_mode_no_procfs.sh.results.xml"
    echo "result file: $result"
    local stderr
    stderr=$(mktemp no_procfs.err.XXXXXX)
    echo "stderr file: $stderr"

    [[ -f "$result" ]] && rm -f "$result"

    # The /proc of this root is empty, as in a disk image.
    tmpdir=$(mktemp -t -d "test_offline_mode_environmentvariable58.XXXXXX")
    mkdir "$tmpdir/proc"

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results "$result" "$definition" 2> "$stderr"

    unset_chroot_offline_test_mode

    [[ -f "$result" ]]

    assert_exists 1 '/oval_results/results/system/tests/test[@result="not applicable"][@test_id="oval:1:tst:1"]'

    grep -Ei "(W: |E: )" "$stderr" && ret_val=1 && echo "There is an error and/or a warning in the output!" >&2

    rm "$stderr"

    rm -rf "${tmpdir}"

    return $ret_val
}

function test_probes_environmentvariable58_offline_mode_process {
    probecheck "environmentvariable58" || return 255

    local definition="${srcdir}/test_probes_environmentvariable58_offline_mode_process.xml"
    local result="test_probes_environmentvariable58_offline_mode_process.results.xml"
    [[ -f "$result" ]] && rm -f "$result"

    # The /proc of this root lists process 42 and no process 1.
    local tmpdir
    tmpdir=$(mktemp -t -d "test_offline_mode_environmentvariable58.XXXXXX")
    mkdir -p "$tmpdir/proc/42"
    printf 'FAKE_VARIABLE=fake value\0' > "$tmpdir/proc/42/environ"

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results "$result" "$definition"

    unset_chroot_offline_test_mode

    rm -rf "$tmpdir"

    [[ -f "$result" ]]

    assert_exists 1 '/oval_results/results/system/tests/test[@result="true"][@test_id="oval:1:tst:1"]'

    return 0
}

test_run "test_probes_environmentvariable58_offline_mode" test_probes_environmentvariable58_offline_mode
test_run "test_probes_environmentvariable58_offline_mode (no procfs)" test_probes_environmentvariable58_offline_mode_no_procfs
test_run "test_probes_environmentvariable58_offline_mode (process)" test_probes_environmentvariable58_offline_mode_process
