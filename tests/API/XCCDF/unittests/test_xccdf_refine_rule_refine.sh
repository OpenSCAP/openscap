#!/bin/bash

# Test <refine-rule> element
# 1st phase use xccdf profile with refine-rules
# 2nd phase generate tailoring file from xccdf and use tailoring file's profiles

# Return XPath expression of <rule-result>
xpath_rule_result() {
	local id_ref="$1"
	echo '//TestResult/rule-result[ @idref="'"${id_ref}"'" ]'
}

# Assert <rule_result> has "notchecked" in <result>
assert_notchecked() {
	local id_ref="$1"
	local rule_result="`xpath_rule_result \"${id_ref}\"`"
	assert_exists 1 "${rule_result}"'/result[ text()="notchecked" ]' > /dev/stdout
}

# Assert <rule_result> has NOT "nonchecked" in <result>
assert_checked() {
	local id_ref="$1"
	local rule_result="`xpath_rule_result \"${id_ref}\"`"
	assert_exists 1 "${rule_result}"'/result[ text()!="notchecked" ]' > /dev/stdout
}

# Assert weight of <rule-result>
assert_weight() {
	local id_ref="$1"
	local weight="$2"
	assert_exists 1 "//rule-result[ @idref=\"${id_ref}\" and starts-with(@weight, \"${weight}\") ]" > /dev/stdout
}

# Create tailoring file from xccdf profiles
# Add "tailor-" prefix before profile names
# Return tailoring file content
create_tailoring() {
	local xccdf_filename="$1"
	echo '<?xml version="1.0" encoding="UTF-8"?>'
	echo '<cdf-11-tailoring:Tailoring xmlns:cdf-11-tailoring="http://open-scap.org/page/Xccdf-1.1-tailoring" xmlns="http://checklists.nist.gov/xccdf/1.1" id="xccdf_scap-workbench_tailoring_default">'
	echo '<cdf-11-tailoring:version time="2014-03-13T12:22:15">1</cdf-11-tailoring:version>'
	cat "$xccdf_filename" |\
		tr '\t' '  ' |\
		tr '\n' '\t '  |\
		sed -E 's;<Profile(.*)</Profile>.*;<NEW><Profile\1</Profile>;' |\
		sed -E 's;^.*<NEW>(.*);\1;' |\
		sed -E 's;"(child|parent|grandparent)";"tailor-\1";g' |\
		tr '\t' '\n'
	echo ''
	echo '</cdf-11-tailoring:Tailoring>'
}


check_results() {
	local result="$1"
	### 1. Grandparent refine selector of rule and child refine role of rule
	#assert_exists 1 "`xpath_rule_result rule-id-selected-and-role`"'//check-content-ref[ @name="oval:moc.elpmaxe.www:def:2" ]'
	assert_checked rule-id-selected-and-role

	### 2. <refine-rule @weight> is two times overrided by extending of profile
	assert_weight rule-id-override-refine 8

	### 3. two refine-rules partially override themselves
	assert_exists 1 '//rule-result[ @idref="rule-id-partially-refined" ]/result[ text()!="notchecked" ]'
	#assert_exists 1 '//rule-result[ @idref="rule-id-partially-refined" and starts-with(@weight, "5") ]/result[ text()!="notchecked" ]'

	### 4. The rule should be checked (no refine-rule is used)
	assert_checked rule-id-checked

	### 5. <refine-rule> enable check
	assert_checked rule-id-checked

	### 6. <refine-rule> from parent profile enable check
	assert_checked rule-id-enable-parent-check

	### 7.
	assert_notchecked rule-id-without-refine

	### 8.
	assert_notchecked rule-id-inherited

	### 9.
	assert_notchecked rule-id-simple

	### 10.
	assert_notchecked rule-id-cluster

	### 11. <refine-rule> for @weight
	assert_weight rule-id-weight 2

	### 12. <refine-rule> for @selector
	assert_exists 1 "`xpath_rule_result rule-id-multiple-check`"'//check-content-ref[ @name="oval:moc.elpmaxe.www:def:2" ]'
	assert_checked rule-id-multiple-check

	### 13. two refine-rules partially override themselves
	assert_exists 1 '//rule-result[ @idref="rule-id-partially-refined-2" ]/result[ text()="notchecked" ]'
	#assert_exists 1 '//rule-result[ @idref="rule-id-partially-refined-2" and starts-with(@weight, "5") ]/result[ text()="notchecked" ]'

	### 14. severity should be redefined
	assert_exists 1 '//rule-result[ @idref="rule-id-severity" and @severity="high" ]'

	### 15. weight should be overrided
	assert_exists 1 '//rule-result[ @idref="rule-id-weight-override" and starts-with(@weight, "4") ]'

	### 16. unscored should be checked
	assert_checked rule-id-unscored
}

set -e
set -o pipefail

name=$(basename $0 .sh)
xccdf="$srcdir/${name}.xccdf.xml"

########################################################################
## Test refine-rules in profiles
########################################################################
echo "Test refine-rules in profiles:"

result=$(mktemp -t ${name}.res.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)

echo "Stderr file = $stderr"
echo "Result file = $result"

$OSCAP xccdf eval --profile child --results $result $xccdf > $stdout 2> $stderr || true #we expect exit code != 0

[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

$OSCAP xccdf validate-xml $result

RULE_COUNT=16

# Same count of results as rules
assert_exists ${RULE_COUNT} '//Rule'
assert_exists ${RULE_COUNT} '//TestResult/rule-result/result'

assert_exists 4 '//Rule[ @role="unchecked" ]' # Refine-rule does not modify rules values directly
check_results $result

rm $result
########################################################################
## Test refine-rules in tailoring file
########################################################################
echo ""
echo "Test refine-rules in tailoring file:"

result=$(mktemp -t ${name}.res.XXXXXX)
stderr=$(mktemp -t ${name}.err.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
tailoring=$(mktemp -t ${name}.in.XXXXXX)

echo "Stderr file = $stderr"
echo "Stdout file = $stdout"
echo "Result file = $result"
echo "Tailoring file = $tailoring"

echo "`create_tailoring \"$xccdf\"`" > $tailoring

$OSCAP xccdf eval --tailoring-file $tailoring --profile tailor-child --results $result $xccdf > $stdout 2> $stderr || true #we expect exit code != 0

check_results $result

[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
rm $result $tailoring
