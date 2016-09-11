#!/bin/bash

result=`mktemp`

set -e
set -o pipefail

# Sanity check - system characteristics are provided by default
$OSCAP oval eval                    --results $result $srcdir/test_without_syschars.xml

assert_exists 1 '//system_data'
assert_exists 1 '//collected_objects'

# Check --without-syschars - no system characteristics expected
$OSCAP oval eval --without-syschars --results $result $srcdir/test_without_syschars.xml

assert_exists 0 '//system_data'
assert_exists 0 '//collected_objects'

rm $result

