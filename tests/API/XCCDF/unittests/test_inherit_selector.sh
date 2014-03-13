#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$name.oval.xml-0.variables-0.xml
stderr=$(mktemp -t ${name}.out.XXXXXX)

$OSCAP xccdf eval --profile test2 --export-variable $srcdir/${name}.xccdf.xml 2> $stderr

echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stderr ]; [ ! -s $stderr ]

assert_exists 1 '//variable[@id="oval:ssg:var:1"]/value[text()="600"]'
assert_exists 1 '//variable[@id="oval:ssg:var:2"]/value[text()="300"]'
assert_exists 1 '//variable[@id="oval:ssg:var:3"]/value[text()="600"]'
:> $result

$OSCAP xccdf eval --profile test2_tailored --export-variables \
	--tailoring-file $srcdir/${name}.tailoring.xml $srcdir/${name}.xccdf.xml 2> $stderr

[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
assert_exists 1 '//variable[@id="oval:ssg:var:1"]/value[text()="600"]'
assert_exists 1 '//variable[@id="oval:ssg:var:2"]/value[text()="300"]'
assert_exists 1 '//variable[@id="oval:ssg:var:3"]/value[text()="600"]'

rm $result

