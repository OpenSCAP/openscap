#!/bin/bash

result=`mktemp`

set -e
set -o pipefail

$OSCAP oval eval --results $result $srcdir/test_platform_version.xml

assert_exists 2 '/oval_results/generator/oval:schema_version'
assert_exists 1 '/oval_results/generator/oval:schema_version[@platform]'
assert_exists 1 '/oval_results/generator/oval:schema_version[@platform="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix"]'
assert_exists 1 '/oval_results/generator/oval:schema_version[text()="5.11.1"]'
assert_exists 1 '/oval_results/generator/oval:schema_version[text()="5.11.1:1.0"]'

assert_exists 2 '/oval_results/oval_definitions/generator/oval:schema_version'
assert_exists 1 '/oval_results/oval_definitions/generator/oval:schema_version[@platform]'
assert_exists 1 '/oval_results/oval_definitions/generator/oval:schema_version[@platform="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix"]'
assert_exists 1 '/oval_results/oval_definitions/generator/oval:schema_version[text()="5.11.1"]'
assert_exists 1 '/oval_results/oval_definitions/generator/oval:schema_version[text()="5.11.1:1.0"]'

assert_exists 2 '/oval_results/results/system/oval_system_characteristics/generator/oval:schema_version'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/generator/oval:schema_version[@platform]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/generator/oval:schema_version[@platform="http://oval.mitre.org/XMLSchema/oval-definitions-5#unix"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/generator/oval:schema_version[text()="5.11.1"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/generator/oval:schema_version[text()="5.11.1:1.0"]'

rm $result

