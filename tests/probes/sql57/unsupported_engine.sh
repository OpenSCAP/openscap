#!/bin/bash

set -e -o pipefail

name=$(basename $0 .sh)
result=$(mktemp ${name}.out.XXXXXX)
stderr=$(mktemp ${name}.err.XXXXXX)
echo "Result file: $result"
echo "stderr file: $stderr"

echo "Evaluating content."
$OSCAP oval eval --results $result $srcdir/${name}.oval.xml 2> $stderr
# filter out the expected error in stderr
sed -i -E "/^E: lt-probe_sql57: DB engine not supported: sqlserver/d" "$stderr"
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
echo "Validating results."
#$OSCAP oval validate-xml --results --schematron $result
echo "Testing results values."
assert_exists 1 '/oval_results'
assert_exists 1 '/oval_results/oval_definitions'
assert_exists 1 '/oval_results/oval_definitions/tests'
assert_exists 1 '/oval_results/oval_definitions/tests/ind-def:sql57_test'
assert_exists 5 '/oval_results/oval_definitions/tests/ind-def:sql57_test/@*'
assert_exists 1 '/oval_results/oval_definitions/tests/ind-def:sql57_test[@id="oval:org.gentoo.dev.swift.postgres:tst:1"]'
assert_exists 1 '/oval_results/oval_definitions/tests/ind-def:sql57_test[@version="1"]'
assert_exists 1 '/oval_results/oval_definitions/tests/ind-def:sql57_test[@check_existence="only_one_exists"]'
assert_exists 1 '/oval_results/oval_definitions/tests/ind-def:sql57_test[@check="all"]'
assert_exists 1 '/oval_results/oval_definitions/tests/ind-def:sql57_test/@comment'
assert_exists 2 '/oval_results/oval_definitions/tests/ind-def:sql57_test/*'
assert_exists 1 '/oval_results/oval_definitions/tests/ind-def:sql57_test/ind-def:object'
assert_exists 1 '/oval_results/oval_definitions/tests/ind-def:sql57_test/ind-def:state'
assert_exists 1 '/oval_results/oval_definitions/objects'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object'
assert_exists 3 '/oval_results/oval_definitions/objects/ind-def:sql57_object/@*'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object[@id="oval:org.gentoo.dev.swift.postgres:obj:1"]'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object[@version="1"]'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/@comment'
assert_exists 4 '/oval_results/oval_definitions/objects/ind-def:sql57_object/*'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:engine'
assert_exists 0 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:engine/@*'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:engine[text()="sqlserver"]'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:version'
assert_exists 0 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:version/@*'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:version[text()="1.0"]'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:connection_string'
assert_exists 0 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:connection_string/@*'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:connection_string[text()]'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:sql'
assert_exists 0 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:sql/@*'
assert_exists 1 '/oval_results/oval_definitions/objects/ind-def:sql57_object/ind-def:connection_string[text()]'
assert_exists 1 '/oval_results/oval_definitions/states'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state'
assert_exists 3 '/oval_results/oval_definitions/states/ind-def:sql57_state/@*'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state[@id="oval:org.gentoo.dev.swift.postgres:ste:1"]'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state[@version="1"]'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/@comment'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/*'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result/@*'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result[@datatype="record"]'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result/*'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result/oval-def:field'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result/oval-def:field/@*'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result/oval-def:field[@name="usename"]'
assert_exists 1 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result/oval-def:field[text()="sqladmin"]'
assert_exists 0 '/oval_results/oval_definitions/states/ind-def:sql57_state/ind-def:result/oval-def:field/*'
assert_exists 1 '/oval_results/results'
assert_exists 1 '/oval_results/results/*'
assert_exists 1 '/oval_results/results/system'
assert_exists 3 '/oval_results/results/system/*'
assert_exists 1 '/oval_results/results/system/definitions'
assert_exists 1 '/oval_results/results/system/definitions/definition'
assert_exists 3 '/oval_results/results/system/definitions/definition/@*'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:org.gentoo.dev.swift.postgres:def:1"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@result="error"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@version="1"]'
assert_exists 1 '/oval_results/results/system/definitions/definition/*'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria'
assert_exists 2 '/oval_results/results/system/definitions/definition/criteria/@*'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria[@result="error"]'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria[@operator="AND"]'
assert_exists 1 '/oval_results/results/system/definitions/definition/criteria/*'
assert_exists 1 '/oval_results/results/system/tests'
assert_exists 1 '/oval_results/results/system/tests/test'
assert_exists 5 '/oval_results/results/system/tests/test/@*'
assert_exists 1 '/oval_results/results/system/tests/test[@result="error"]'
assert_exists 0 '/oval_results/results/system/tests/test/*'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object'
assert_exists 3 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/@*'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:org.gentoo.dev.swift.postgres:obj:1"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@version="1"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@flag="error"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/*'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/message/@*'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/message[@level="error"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/message[text()="DB engine not supported: sqlserver"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/collected_objects/object/message/*'

rm $result
