#!/bin/bash

########################################################################
### Test "oscap xccdf resolve" command
### Should increase code lines coverage
########################################################################

set -e
set -o pipefail

name=$(basename $0 .sh)

result=$(mktemp -t ${name}.res.XXXXXX)
stderr=$(mktemp -t ${name}.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)


echo "Stderr file = $stderr"
echo "Result file = $result"

$OSCAP xccdf resolve --output $result $srcdir/${name}.xccdf.xml > $stdout
$OSCAP xccdf validate $result >> $stdout

assert_exists 1 '//Benchmark[@resolved="1"]' 

# Grand parent
assert_exists 2 '//Profile[@id="xccdf_resolve_profile_grandparent"]/select' 
assert_exists 1 '//Profile[@id="xccdf_resolve_profile_grandparent"]/select[@idref="xccdf_test_rule_inherited" and @selected="true"]'
assert_exists 1 '//Profile[@id="xccdf_resolve_profile_grandparent"]/select[@idref="xccdf_test_rule_overridden" and @selected="true"]'

# Parent
assert_exists 2 '//Profile[@id="xccdf_resolve_profile_parent"]/select' # 2 selects from grand parent + 0 own
assert_exists 1 '//Profile[@id="xccdf_resolve_profile_parent"]/select[@idref="xccdf_test_rule_inherited" and @selected="true"]'
assert_exists 1 '//Profile[@id="xccdf_resolve_profile_parent"]/select[@idref="xccdf_test_rule_overridden" and @selected="false"]'

# Child
assert_exists 3 '//Profile[@id="xccdf_resolve_profile_child"]/select' # 2 selects from parent + 1 own
assert_exists 1 '//Profile[@id="xccdf_resolve_profile_child"]/select[@idref="xccdf_test_rule_inherited" and @selected="true"]'
assert_exists 1 '//Profile[@id="xccdf_resolve_profile_child"]/select[@idref="xccdf_test_rule_overridden" and @selected="false"]'
assert_exists 1 '//Profile[@id="xccdf_resolve_profile_child"]/select[@idref="xccdf_test_rule_own" and @selected="true"]'

[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr
rm $result
