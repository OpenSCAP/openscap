#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$name.oval.xml-0.variables-0.xml
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --export-variable $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]

assert_exists() { [ $($XPATH $result 'count('$2')') == "$1" ]; }
test_variable() {
	assert_exists 3 '/oval_variables/variables/variable'
	assert_exists 3 '//variable'
	assert_exists 3 '//variable/*'
	assert_exists 3 '//variable/value'
	assert_exists 0 '//variable/value/@*'
	assert_exists 1 '//variable[@id="oval:ssg:var:1"]/value'
	assert_exists 1 '//variable[@id="oval:ssg:var:2"]/value'
	assert_exists 1 '//variable[@id="oval:ssg:var:3"]/value'
	assert_exists 3 '//variable/value[text()="100"]'
}

test_variable
rm -rf $result

# Ensure that it works even for export-oval-variables (trac#267)
$OSCAP xccdf export-oval-variables $srcdir/${name}.xccdf.xml 2>&1 > $stderr
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
test_variable
rm $result

