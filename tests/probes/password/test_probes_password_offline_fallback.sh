#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_password_offline_fallback {
	probecheck "password" || return 255

	# This regression targets platforms without fgetpwent(3), primarily macOS.
	case "$(uname)" in
		Darwin) ;;
		*) return 255 ;;
	esac

	local definition_file="${srcdir}/test_probes_password_offline.xml"
	local results_file="results.xml"
	[[ -f "$results_file" ]] && rm -f "$results_file"

	tmpdir=$(mktemp -t -d "test_password_fallback.XXXXXX")
	mkdir -p "$tmpdir/etc"
	cat > "$tmpdir/etc/passwd" <<'EOF'
# comment line should be ignored

invalid_line_without_separators
root:x:0:0:root:/root:/bin/bash
EOF

	set_offline_chroot_dir "$tmpdir"
		$OSCAP oval eval --results "$results_file" "$definition_file"
	set_offline_chroot_dir ""
	rm -rf "$tmpdir"

	if [[ -f "$results_file" ]]; then
		result="$results_file"
		assert_exists 1 'oval_results/results/system/tests/test[@test_id="oval:1:tst:1"][@result="true"]'
	else
		return 1
	fi
}

test_init

test_run "test_probes_password_offline_fallback" test_probes_password_offline_fallback

test_exit
