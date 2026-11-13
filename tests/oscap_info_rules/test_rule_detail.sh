#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

ds="$srcdir/ds.xml"
ds_multi_checklist="$srcdir/r600-datastream.xml"
xccdf_checklist="$srcdir/test_tailoring_benchmark.xml"


r_datastream="xccdf_moc.elpmaxe.www_rule_1"
r_datastream_multi="xccdf_gov.nist_rule_xccdf_gov.nist_rule-1"
r_checklist="xccdf_com.example.www_rule_R6"



r_invalid="xccdf_com.example.www_rule_INVALID"


# Test 1: datastream with a single checklist, --rule with a correct ID prints selected rule details

stdout=$(mktemp)
stderr=$(mktemp)
$OSCAP info --rule $r_datastream $ds > $stdout 2> $stderr || ret=$?
[ $ret =  0 ] 
[ ! -s stderr ]
[ -s stdout ]
grep -q "This is a rule description" $stdout
grep -q "This is a rule rationale" $stdout
grep -q "This is a rule warning" $stdout
rm -f $stderr
rm -f $stdout



# Test 2: datastream with multiple checklists, --rule with a correct ID prints selected rule details
stdout=$(mktemp)
stderr=$(mktemp)
$OSCAP info --rule $r_datastream_multi $ds_multi_checklist > $stdout 2> $stderr || ret=$?
[ $ret =  0 ] 
[ ! -s stderr ]
[ -s stdout ]
grep -q "Make sure the unknown test type returns properly" $stdout
grep -q "This is a warning" $stdout
grep -q "This is a rationale" $stdout
grep -q "This is fixtext" $stdout
grep -q "This is check-content" $stdout
rm -f $stderr
rm -f $stdout


# Test 3: checklist --rule with a correct ID prints specified rule details
stdout=$(mktemp)
stderr=$(mktemp)
$OSCAP info --rule $r6 $xccdf_checklist > $stdout 2> $stderr || ret=$?
[ $ret =  0 ]
[ ! -s stderr ]
[ -s stdout ]
grep -q "Rule R6" $stdout
grep -q "High" $stdout
grep -q "This is a rule description" $stdout
grep -q "This is a rule warning" $stdout
grep -q "This is a rule rationale" $stdout
grep -q "This is fixtext" $stdout
grep -q "This is check-content" $stdout
rm -f $stderr
rm -f $stdout

    

# Test 4: --rule without an ID produces an error
$OSCAP info --rule $ds > $stdout 2> $stderr || ret=$?
[ $ret =  1 ]


# Test 5: --rule without an ID present in the input file produces a failure
$OSCAP info --rule $r_invalid $ds > $stdout 2> $stderr
[ $ret =  2 ]

