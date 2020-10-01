#!/usr/bin/env bash

. $builddir/tests/test_common.sh

function test_probes_yamlfilecontent_key {

    probecheck "yamlfilecontent" || return 255

    local ret_val=0
    local DF="${srcdir}/test_probes_yamlfilecontent_key.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

    cp "${srcdir}/openshift-logging.yaml" /tmp

    local YAML_FILE="/tmp/openshift-logging.yaml"

    $OSCAP oval eval --results $RF $DF

    if [ -f $RF ]; then
        verify_results "def" $DF $RF 6 && verify_results "tst" $DF $RF 7
        ret_val=$?
    else
        ret_val=1
    fi

    rm -f $YAML_FILE

    return $ret_val
}

test_probes_yamlfilecontent_key
