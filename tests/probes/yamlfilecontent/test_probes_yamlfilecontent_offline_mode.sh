#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_yamlfilecontent_offline_mode {

    probecheck "yamlfilecontent" || return 255

    local ret_val=0
    local DF="${srcdir}/test_probes_yamlfilecontent_offline_mode.xml"
    local RF="results.xml"
    local YAML_FILE="openshift-logging.yaml"

    [ -f $RF ] && rm -f $RF

    tmpdir=$(mktemp -t -d "test_offline_mode_yamlfilecontent.XXXXXX")

    # Setup chroot fs and host with test files in different states
    mkdir $tmpdir/tmp
    cp "${srcdir}/${YAML_FILE}" "${tmpdir}/tmp"

    cp "${srcdir}/${YAML_FILE}" /tmp
    sed -i 's/name: instance/name: outstance/' "/tmp/${YAML_FILE}"
    cp "${srcdir}/${YAML_FILE}" "/tmp/host-${YAML_FILE}"

    set_chroot_offline_test_mode "$tmpdir"

    $OSCAP oval eval --results $RF $DF

    unset_chroot_offline_test_mode

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 3 && verify_results "tst" $DF $RF 3
        ret_val=$?
    else
        ret_val=1
    fi

    rm -f /tmp/$YAML_FILE
    rm -rf ${tmpdir}

    return $ret_val
}

test_probes_yamlfilecontent_offline_mode
