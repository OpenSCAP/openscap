#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_shadow_stripped {

    probecheck "shadow" || return 255

    local ret_val=0
    local DF="${srcdir}/test_probes_shadow_stripped.xml"
    local RF="$(mktemp results.XXXXXXX.xml)"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(make_temp_dir /tmp "test_probes_shadow_stripped")
    mkdir -p "${tmpdir}/etc"
    cat > "${tmpdir}/etc/shadow" << 'SHADOW'
sha512user:$6$saltsalt$longhashvaluethatneedstoberedacted:19000:0:99999:7:::
lockedhash:!!$6$anothersalt$anotherlonghashvalue:19000:0:99999:7:::
lockednohash:!:19000:0:99999:7:::
disabled:*:19000:0:99999:7:::
neverset:!!:19000:0:99999:7:::
bsdiuser:_bsdihashvalueredact1:19000:0:99999:7:::
bsdilocked:!_bsdihashvalueredact2:19000:0:99999:7:::
sunmd5user:$md5,rounds=4294963199$saltvalueredact$$hashvalueredacted:19000:0:99999:7:::
descryptuser:aZ4ZloVToj1nA:19000:0:99999:7:::
descryptlocked:!aZ4ZloVToj1nA:19000:0:99999:7:::
SHADOW

    export OSCAP_PROBE_ROOT="${tmpdir}"

    $OSCAP oval eval --results $RF $DF

    unset OSCAP_PROBE_ROOT
    rm -rf "${tmpdir}"

    if [ -f $RF ]; then
	verify_results "def" $DF $RF 10 && verify_results "tst" $DF $RF 10
	ret_val=$?
    else
	ret_val=1
    fi

    if grep -q 'longhashvaluethatneedstoberedacted\|anotherlonghashvalue\|saltsalt\|anothersalt\|bsdihashvalueredact1\|bsdihashvalueredact2\|saltvalueredact\|hashvalueredacted\|aZ4ZloVToj1nA' $RF; then
        ret_val=1
    fi

    rm -f $RF
    return $ret_val
}

test_init

test_run "test_probes_shadow_stripped" test_probes_shadow_stripped

test_exit
