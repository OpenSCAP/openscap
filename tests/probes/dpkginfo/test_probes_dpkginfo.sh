#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_dpkginfo {
	probecheck "dpkginfo" || return 255

	local root_dir
	local result
	local ret_val=0

	root_dir=$(mktemp -d)
	result=$(mktemp)
	mkdir -p "$root_dir/var/lib/dpkg"

	cat > "$root_dir/var/lib/dpkg/status" <<'EOF'
Package: alpha-tool
Status: install ok installed
Architecture: amd64
Version: 1:1.2.3-4
Provides: system-logger
Description: alpha tool

Package: alpha-lib
Status: install ok installed
Architecture: all
Version: 2.0-1
Description: alpha library

Package: beta-tool
Status: install ok installed
Architecture: amd64
Version: 3.0-1
Description: beta tool

Package: alpha-old
Status: deinstall ok config-files
Architecture: amd64
Version: 0.1-1
Description: removed alpha package
EOF

	OSCAP_PROBE_ROOT="$root_dir" $OSCAP oval eval --results "$result" "$srcdir/test_probes_dpkginfo.xml"

	if [ -f "$result" ]; then
		verify_results "def" "$srcdir/test_probes_dpkginfo.xml" "$result" 4 && \
			verify_results "tst" "$srcdir/test_probes_dpkginfo.xml" "$result" 4
		ret_val=$?
	else
		ret_val=1
	fi

	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:1:obj:1"]/reference' || ret_val=1
	assert_exists 2 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:1:obj:2"]/reference' || ret_val=1
	assert_exists 3 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:1:obj:3"]/reference' || ret_val=1
	assert_exists 1 '/oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:1:obj:4"]/reference' || ret_val=1

	rm -rf "$root_dir"
	rm -f "$result"

	return $ret_val
}

test_init

test_run "dpkginfo probe name operations" test_probes_dpkginfo

test_exit
