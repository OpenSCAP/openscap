#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.output.XXXXXX)
stderr=$(mktemp -t ${name}.stderr.XXXXXX)
stdout=$(mktemp -t ${name}.stdout.XXXXXX)

# We cannot simply unset, because there still might be a non empty system wide location.
# We can set some nonsence instead.
OSCAP_XSLT_PATH=/blah
export OSCAP_XSLT_PATH
$OSCAP xccdf generate report --output $result $srcdir/test_report_check_with_empty_selector.xccdf.xml.result.xml 2> $stderr > $stdout || [ $? == 1 ]

echo "Stdout file = $stdout"
echo "Stderr file = $stderr"
echo "Result file = $result"
[ -f $stdout ]; [ ! -s $stdout ]; rm $stdout
[ -f $result ]; [ ! -s $result ]; rm $result
[ -f $stderr ]; grep "XSLT file 'xccdf-report.xsl' not found in path" $stderr; rm $stderr
