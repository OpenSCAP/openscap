#!/bin/bash

# Test loading of OVAL files located relatively to xccdf file

set -e
set -o pipefail

name=$(basename $0 .sh)

XCCDF="$(realpath "$srcdir/${name}.xccdf.xml")"

# cd to directory and run test
function testInDirectory(){
	local directory="$1"

	pushd "$directory"

	local result=$(mktemp -t ${name}.out.XXXXXX)
	local stderr=$(mktemp -t ${name}.out.XXXXXX)

	echo "Stderr file = $stderr"
	echo "Result file = $result"

	$OSCAP xccdf eval --results $result --oval-results "$XCCDF" 2> $stderr 

	[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

	assert_exists 2 '//rule-result/result'
	assert_exists 2 '//rule-result/result[text()="pass" or text()="fail"]'

	rm $result

	popd
}

# test in XCCDF's directory
XCCDF_DIR="$(dirname "$XCCDF")"
testInDirectory "$XCCDF_DIR"

# test in random directory
RANDOM_DIRECTORY=`mktemp -d`
testInDirectory $RANDOM_DIRECTORY
rmdir $RANDOM_DIRECTORY




