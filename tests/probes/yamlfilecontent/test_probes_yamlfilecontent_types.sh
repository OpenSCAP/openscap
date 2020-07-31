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

    sd='/oval_results/results/system/oval_system_characteristics/system_data'

    assert_exists 8 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="boolean"]'
    assert_exists 4 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="boolean" and text()="true"]'
    assert_exists 4 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="boolean" and text()="false"]'

    assert_exists 5 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="int"]'
    # int_10: 42
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="int" and text()="42"]'
    # int_10_neg: -17
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="int" and text()="-17"]'
    # int_8: 0o33
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="int" and text()="27"]'
    # int_16: 0xFF
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="int" and text()="255"]'
    # int_cast: !!int "369"
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="int" and text()="369"]'

    assert_exists 7 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="float"]'
    # float: 7.4
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="float" and text()="7.400000"]'
    # float_neg: -0.3
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="float" and text()="-0.300000"]'
    # float_exp: +12e03
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="float" and text()="12000.000000"]'
    # float_exp_neg: -43e-4
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="float" and text()="-0.004300"]'
    # float: .inf
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="float" and text()="inf"]'
    # float: .NAN
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="float" and text()="nan"]'
    # float_cast: !!float "978.65"
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype="float" and text()="978.650000"]'

    # string_true
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype!="boolean" and text()="true"]'
    # string_number
    assert_exists 1 $sd'/ind-sys:yamlfilecontent_item/ind-sys:value/field[@name="" and @datatype!="int" and text()="81"]'

    # bool_error_cast, int_error_cast, float_error_cast
    co='/oval_results/results/system/oval_system_characteristics/collected_objects'
    assert_exists 3 $co'/object[@flag="error"]'
    assert_exists 3 $co'/object[@flag="error"]/message'

    rm -f $YAML_FILE
}

test_probes_yamlfilecontent_types
