#!/usr/bin/env bash

. "$builddir/tests/test_common.sh"

set -e -o pipefail

tmpdir=""

function test_probes_shadow_yescrypt {
	probecheck "shadow" || return 255
	[[ $(uname) == Linux ]] || return 255

	tmpdir=$(make_temp_dir /tmp "test_shadow_yescrypt")
	trap 'rm -rf "$tmpdir"' EXIT
	mkdir -p "$tmpdir/etc"
	cp "$srcdir/test_probes_shadow_yescrypt.shadow" "$tmpdir/etc/shadow"
	set_offline_chroot_dir "$tmpdir"

	local definition="$tmpdir/definitions.xml"
	local result="$tmpdir/results.xml"
	local items='//*[local-name()="shadow_item"]'
	local invalid_items="$items[*[local-name()='username' and starts-with(., 'invalid_')]]"
	local hashless_items="$items[*[local-name()='username' and starts-with(., 'hashless_')]]"
	local schema_version

	for schema_version in 5.8 5.9 5.10 5.10.1 5.11 5.11.1 5.11.2 5.11.3; do
		sed "s/SCHEMA_VERSION_PLACEHOLDER/$schema_version/" \
			"$srcdir/test_probes_shadow_yescrypt.xml" > "$definition"
		local modern_count
		case $schema_version in
			5.8|5.9|5.10|5.10.1)
				xsed -i '/OVAL_5_11_ENCRYPT_METHOD/d' "$definition"
				modern_count=0
				;;
			5.11|5.11.1|5.11.2|5.11.3)
				modern_count=4
				;;
			*)
				printf 'Unexpected OVAL schema version: %s\n' "$schema_version" >&2
				return 1
				;;
		esac
		$OSCAP oval eval --results "$result" "$definition"

		verify_results "def" "$definition" "$result" 1
		verify_results "tst" "$definition" "$result" 8
		assert_exists 36 "$items"
		local method method_items
		for method in yescrypt gost-yescrypt sm3-yescrypt scrypt; do
			method_items="$items[starts-with(*[local-name()='username'], '${method}_')]"
			assert_exists 4 "$method_items"
			assert_exists "$modern_count" "$method_items/*[local-name()='encrypt_method']"
			assert_exists "$modern_count" \
				"$method_items/*[local-name()='encrypt_method' and text()='$method']"
		done
		assert_exists 1 "$items/*[local-name()='encrypt_method' and text()='SHA-512']"
		assert_exists 2 "$items/*[local-name()='encrypt_method' and text()='DES']"
		assert_exists 9 "$invalid_items"
		assert_exists 0 "$invalid_items/*[local-name()='encrypt_method']"
		assert_exists 8 "$hashless_items"
		assert_exists 0 "$hashless_items/*[local-name()='encrypt_method']"
	done

	# OVAL versions before 5.8 do not support encrypt_method.
	sed 's/SCHEMA_VERSION_PLACEHOLDER/5.4/' \
		"$srcdir/test_probes_shadow_yescrypt.xml" > "$definition"
	xsed -i '/<unix-def:encrypt_method>/d' "$definition"
	$OSCAP oval eval --results "$result" "$definition"
	verify_results "def" "$definition" "$result" 1
	verify_results "tst" "$definition" "$result" 8
	assert_exists 36 "$items"
	assert_exists 0 "$items/*[local-name()='encrypt_method']"

	set_offline_chroot_dir ""
}

test_init
test_run "test_probes_shadow_yescrypt" test_probes_shadow_yescrypt
test_exit
