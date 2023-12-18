#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

result=$(mktemp)
root="/tmp/oscap_test_skip_paths"
mkdir -p "$root/a"
touch "$root/a/x"
cp "$srcdir/test.xml" "$root/a/"
mkdir -p "$root/b"
touch "$root/b/y"
cp "$srcdir/test.xml" "$root/b/"
mkdir -p "$root/c"
touch "$root/c/z"
cp "$srcdir/test.xml" "$root/c/"
# oscap probes will skip directories "b" and "c"
export OSCAP_PROBE_IGNORE_PATHS="$root/b:$root/c"
$OSCAP oval eval --results $result "$srcdir/test_skip_paths.xml"
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1" and @result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:1" and @flag="complete"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:file_item'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:file_item/unix-sys:filepath[text()="/tmp/oscap_test_skip_paths/a/x"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:2" and @result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:2" and @flag="does not exist"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:file_item/unix-sys:filepath[text()="/tmp/oscap_test_skip_paths/b/y"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:file_item/unix-sys:filepath[text()="/tmp/oscap_test_skip_paths/c/z"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:3" and @result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:3" and @flag="complete"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/a/x"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:4" and @result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:4" and @flag="does not exist"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/b/y"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:textfilecontent_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/c/z"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:5" and @result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:5" and @flag="complete"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:filehash58_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/a/x"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:6" and @result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:6" and @flag="does not exist"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:filehash58_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/b/y"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:filehash58_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/c/z"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:7" and @result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:7" and @flag="complete"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/a/test.xml"]'
assert_exists 1 '/oval_results/results/system/definitions/definition[@definition_id="oval:x:def:8" and @result="true"]'
assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:x:obj:8" and @flag="does not exist"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/b/test.xml"]'
assert_exists 0 '/oval_results/results/system/oval_system_characteristics/system_data/ind-sys:xmlfilecontent_item/ind-sys:filepath[text()="/tmp/oscap_test_skip_paths/c/test.xml"]'
rm -f $result
rm -rf "$root"
