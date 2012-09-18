#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

for args in "" "--profile xccdf_moc.elpmaxe.www_profile_1"; do
	result=$(mktemp -t ${name}.out.XXXXXX)
	stderr=$(mktemp -t ${name}.out.XXXXXX)
	# If an <xccdf:Rule> contains an <xccdf:complex-check>, then the benchmark consumer MUST process it
	# and MUST ignore any <xccdf:check> elements that are also contained by the <xccdf:Rule>.
	$OSCAP xccdf eval --skip-valid --results $result $srcdir/${name}.xccdf.xml 2> $stderr || [ $? == 2 ]

	echo "Stderr file = $stderr"
	echo "Result file = $result"
	[ -s $stderr ] && rm -rf $stderr

	$OSCAP xccdf validate-xml $result || [ $? == 2 ]

	assert_exists_once() { [ $(xpath $result 'count('$1')') == "1" ]; }
	assert_exists_once '//rule-result'
	assert_exists_once '//rule-result/result/text() == "pass"'
	assert_exists_once '//rule-result[not(check)]'
	assert_exists_once '//rule-result/complex-check'
	assert_exists_once '//rule-result/complex-check/check'
	assert_exists_once '//rule-result/complex-check/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]'
	assert_exists_once '//rule-result/complex-check/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]/check-content-ref'
	assert_exists_once '//rule-result/complex-check/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]/check-content-ref[@name="oval:moc.elpmaxe.www:def:1"]'
	assert_exists_once '//rule-result/complex-check/check[@system="http://oval.mitre.org/XMLSchema/oval-definitions-5"]/check-content-ref[@href="test_xccdf_check_content_ref_without_name_attr.oval.xml"]'
	assert_exists_once '//Rule/complex-check[not(@system)]'
	assert_exists_once '//rule-result/complex-check[not(@system)]'
	rm -rf $result
done
