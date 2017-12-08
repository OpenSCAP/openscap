#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
name="${name%_stigw}"
result="$(mktemp -t ${name}.out.XXXXXX)"
stderr=$(mktemp -t ${name}.out.XXXXXX)
ret=0
echo "Stderr file = $stderr"
echo "Result file = $result"


#### XCCDF test cases ####

# Tests that all rules from profile $prof1 (profile contains only 2 rules) are
# evaluated when '--rule' option is not specified.

# One of the rules is supposed to fail, so the return code of this line has to be 0 so the test can continue
$OSCAP xccdf eval --stig-viewer "$result" "$srcdir/${name}.xccdf.xml" 2> "$stderr" || true
[ -f $stderr ]; [ ! -s $stderr ]; :> $stderr

"${PYTHON:-python}" "$srcdir/stig-viewer-equivalence.py" "$result" "$srcdir/correct_stigw_result.xml" 2> "$stderr" || ret=$?
rm "$result"
