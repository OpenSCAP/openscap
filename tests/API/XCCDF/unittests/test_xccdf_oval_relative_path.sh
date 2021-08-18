#!/usr/bin/env bash
. $builddir/tests/test_common.sh

# Test loading of OVAL results files located relatively to XCCDF file

set -e
set -o pipefail

# cd to directory and run test
function testInDirectory(){
	local directory="$1"
	local xccdf="$2"

	pushd "$directory"

	local result=$(make_temp_file /tmp ${name}.out.result)
	local report=$(make_temp_file /tmp ${name}.out.report)
	local stderr=$(make_temp_file /tmp ${name}.err)

	echo "Stderr file = $stderr"
	echo "Result file = $result"
	echo "Report file = $report"

	$OSCAP xccdf eval --oval-results --results $result --report $report "$xccdf" 2> $stderr 

	# Check existence of oval-results
	[ -f $directory/*oval*always-fail*oval.xml.result.xml ]

	[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

	# Check result
	assert_exists 2 '//rule-result/result'
	assert_exists 2 '//rule-result/result[text()="pass" or text()="fail"]'

	# Check report
	grep "Testing permissions on /not_executable" "$report" --quiet

	rm -f $result $report

	popd
}

name=$(basename $0 .sh)
ORIGINAL_XCCDF="$(readlink -f "$srcdir/${name}.xccdf.xml")"

### test in XCCDF's directory

# We don't have write access to directory with XCCDF (after make distcheck),
# so we have to copy required files to directory with right access and do tests there
testDir=$(make_temp_dir /tmp tmp)
mkdir -p "$testDir/oval/always-fail"
cp "$ORIGINAL_XCCDF" "$srcdir/test_default_selector.oval.xml" "$testDir"
cp "$srcdir/oval/always-fail/oval.xml" "$testDir/oval/always-fail"

XCCDF="$(readlink -f "$testDir/${name}.xccdf.xml")"
testInDirectory "$testDir" "$XCCDF"
rm -rf "$testDir"


### test in random directory - different from directory of XCCDF
RANDOM_DIRECTORY=`mktemp -d`
testInDirectory "$RANDOM_DIRECTORY" "$ORIGINAL_XCCDF"
rm -rf $RANDOM_DIRECTORY
