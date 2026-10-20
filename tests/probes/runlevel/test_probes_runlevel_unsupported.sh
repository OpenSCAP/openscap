#!/usr/bin/env bash

. $builddir/tests/test_common.sh

set -e -o pipefail

function test_probes_runlevel_unsupported {
	probecheck "runlevel" || return 255

	case "$(uname)" in
		Darwin|FreeBSD) ;;
		*) return 255 ;;
	esac

	local definition="${top_srcdir}/tests/oval_details/runlevel.oval.xml"
	local results="results_unsupported.xml"
	[[ -f "$results" ]] && rm -f "$results"

	$OSCAP oval eval --results "$results" "$definition"

	result="$results"
	assert_exists 1 'oval_results/results/system/definitions/definition[@definition_id="oval:x:def:1"][@result="error"]'
	assert_exists 1 'oval_results/results/system/tests/test[@test_id="oval:x:tst:1"][@result="error"]'
	assert_exists 1 'oval_results/results/system/oval_system_characteristics/collected_objects/object[@id="oval:ssg:obj:1313"][@flag="error"]'
	assert_exists 1 'oval_results/results/system/oval_system_characteristics/collected_objects/object/message[text()="get_runlevel failed."]'
}

test_init

test_run "test_probes_runlevel_unsupported" test_probes_runlevel_unsupported

test_exit
