#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_shadow_offline_unsupported {
	probecheck "shadow" || return 255

	case "$(uname)" in
		Darwin) ;;
		*) return 255 ;;
	esac

	local definition="${srcdir}/test_probes_shadow_offline.xml"
	local results="results_unsupported.xml"
	[[ -f "$results" ]] && rm -f "$results"

	tmpdir=$(make_temp_dir /tmp "test_offline_mode_shadow_unsupported")
	mkdir -p "${tmpdir}/etc"
	echo "root:!locked::0:99999:7:::" > "${tmpdir}/etc/shadow"
	set_offline_chroot_dir "${tmpdir}"

	$OSCAP oval eval --results "$results" "$definition"

	set_offline_chroot_dir ""
	rm -rf "${tmpdir}"

	result="$results"
	assert_exists 1 'oval_results/results/system/definitions/definition[@definition_id="oval:1:def:1"][@result="not applicable"]'
	assert_exists 1 'oval_results/results/system/tests/test[@test_id="oval:1:tst:1"][@result="not applicable"]'
	assert_exists 1 'oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:1:obj:1"][@flag="not applicable"]'
}

test_init

test_run "test_probes_shadow_offline_unsupported" test_probes_shadow_offline_unsupported

test_exit
