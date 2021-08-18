#!/usr/bin/env bash

set -e -o pipefail

. $builddir/tests/test_common.sh

function curl_accept_encoding {
	local DF="${srcdir}/ds.xml"
	local RF="results.xml"
	local LOG="verbose.log"

	$OSCAP xccdf --verbose=DEVEL eval --fetch-remote-resources --results $RF $DF 2>$LOG || echo "OK"

	case $(uname) in
		FreeBSD)
			grep -iE "Accept-Encoding.*gzip" "$LOG"
			;;
		*)
			grep -iP "Accept-Encoding.*gzip" "$LOG"
			;;
	esac || die "Couldn't assert gzip encoding in the log: $(grep -i 'curl header' $LOG)"

	return 0
}

test_init

test_run "cURL: Accept-Encoding" curl_accept_encoding

test_exit
