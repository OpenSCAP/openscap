#!/bin/bash

set -e -o pipefail

. $builddir/tests/test_common.sh

function curl_accept_encoding {
	local DF="${srcdir}/ds.xml"
	local RF="results.xml"
	local LOG="verbose.log"

	$OSCAP xccdf --verbose=DEVEL eval --fetch-remote-resources --results $RF $DF 2>$LOG || echo "OK"

	grep -P "Accept-Encoding.*gzip" $LOG

	return 0
}

test_init

test_run "cURL: Accept-Encoding" curl_accept_encoding

test_exit
