#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

oval_def=$(mktemp)
result=$(mktemp)
stdout=$(mktemp)
stderr=$(mktemp)
temp_dir=$(mktemp -d)

cleanup() {
	rm -f "$oval_def" "$result" "$stdout" "$stderr"
	rm -rf "$temp_dir"
}
trap cleanup EXIT

cp "$srcdir/test_variable_in_filter.xml" "$oval_def"
sed -i "s;TEMP_DIR_PLACEHOLDER;$temp_dir;" "$oval_def"
echo "secret_key" > "$temp_dir/key_file"

$OSCAP oval eval --results "$result" "$oval_def" > "$stdout" 2> "$stderr"
grep "Failed to convert OVAL state to SEXP" "$stderr" && exit 1
assert_exists 1 '//oval_results/results/system/definitions/definition[@result="true"]'
assert_exists 0 '//oval_results/results/system/definitions/definition[@result!="true"]'
