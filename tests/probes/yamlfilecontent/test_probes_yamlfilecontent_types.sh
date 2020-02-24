#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_yamlfilecontent_types {

    probecheck "yamlfilecontent" || return 255

    local ret_val=0
    local oval_file="${srcdir}/test_probes_yamlfilecontent_types.xml"
    local result="results.xml"

    [ -f $result ] && rm -f $result

    cp "${srcdir}/types.yaml" /tmp

    local YAML_FILE="/tmp/types.yaml"

    $OSCAP oval eval --results $result $oval_file

    [ -f $result ]

    sd='/oval_results/results/system/oval_system_characteristics/system_data/'
    assert_exists 8 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value_of[@datatype="boolean"]'
    assert_exists 5 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value_of[@datatype="int"]'
    assert_exists 7 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value_of[@datatype="float"]'

    rm -f $result
    rm -f $YAML_FILE

}

test_probes_yamlfilecontent_types
