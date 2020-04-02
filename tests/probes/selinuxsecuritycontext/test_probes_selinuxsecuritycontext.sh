#!/usr/bin/env bash

. $builddir/tests/test_common.sh

function test_probes_selinuxsecuritycontext {
    # if the user doesn't have SELinux enabled we will skip the test
    selinuxenabled || return 255

    probecheck "selinuxboolean" || return 255

    local ret_val=0;
    local DF="${srcdir}/test_probes_selinuxsecuritycontext.xml"
    local RF="test_probes_selinuxsecuritycontext.results.xml"
    local stderr=$(mktemp $1.err.XXXXXX)
    echo "stderr file: $stderr"

    [ -f $RF ] && rm -f $RF

    $OSCAP oval eval --results $RF $DF 2>$stderr

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 1
        ret_val=$?
    else
        ret_val=1
    fi

    grep -Ei "(W: |E: )" $stderr && ret_val=1 && echo "There is an error and/or a warning in the output!"
    rm $stderr

    return $ret_val
}

test_run "Basic selinuxsecuritycontext probe test" test_probes_selinuxsecuritycontext
