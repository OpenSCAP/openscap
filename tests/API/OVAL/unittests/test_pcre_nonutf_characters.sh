#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e -o pipefail

oval_def="$(mktemp)"
stdout="$(mktemp)"
stderr="$(mktemp)"
result="$(mktemp)"
temp_dir="$(mktemp -d)"
cp $srcdir/test_pcre_nonutf_characters.xml "$oval_def"
sed -i "s;TEMP_DIR_PLACEHOLDER;$temp_dir;" "$oval_def"
normal_file="$temp_dir/normal_filename"
touch "$normal_file"
evil_file=$temp_dir/$(printf "evil_filename_\334_non_utf8_character")
touch "$evil_file"

$OSCAP oval eval --results "$result" "$oval_def" >"$stdout" 2>"$stderr"

assert_exists 2 '/oval_results/results/system/oval_system_characteristics/system_data/unix-sys:file_item'
grep -q "Definition oval:org.mitre.oval.test:def:1: true" "$stdout"
grep -q "W: oscap: Replaced invalid UTF-8 byte sequence(s) with the replacement character (U+FFFD) in .*" "$stderr"

rm -f "$oval_def"
rm -f "$stdout" "$stderr" "$result"
rm -rf "$temp_dir"
