#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
result=$(mktemp -t ${name}.out.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)

cp $srcdir/${name}.xccdf.xml $result

echo "Stderr file = $stderr"
echo "Result and input file = $result"
tmpdir=$(dirname $result)

for i in {1..5}; do
	$OSCAP xccdf eval --results $result $result 2> $stderr
	[ -f $stderr ]
	[ "`cat $stderr`" == "WARNING: Skipping $tmpdir/non_existent.oval.xml file which is referenced from XCCDF content" ]
	:> $stderr

	$OSCAP xccdf validate $result

	assert_exists $i '//TestResult'
	assert_exists $i '//TestResult/rule-result/result[text()="notchecked"]'
	assert_exists $i '//TestResult/score'
	assert_exists 1 '//TestResult[@id="xccdf_org.open-scap_testresult_default-profile"]'
	assert_exists $i '//TestResult[contains(@id, "xccdf_org.open-scap_testresult_default-profile")]'
	let n=i-1 || true
	assert_exists $n '//TestResult[contains(@id, "xccdf_org.open-scap_testresult_default-profile00")]'
	for j in `seq 1 $n`; do
		assert_exists 1 '//TestResult[contains(@id, "xccdf_org.open-scap_testresult_default-profile00'$j'")]'
	done
done

rm $result
