#!/bin/bash

set -x
set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
stdout=$(mktemp -t ${name}.out.XXXXXX)
result=$(mktemp -t ${name}.out.XXXXXX)
script=$(mktemp -t ${name}.sh.XXXXXX)
benchmark=$srcdir/${name}.xccdf.xml
profile="xccdf_moc.elpmaxe.www_profile_standard"
profile2="xccdf_moc.elpmaxe.www_profile_common"
echo "Stderr file = $stderr"
echo "Result file = $result"
ret=0

# First evaluate benchmark
$OSCAP xccdf eval --profile $profile --profile $profile2 --results-arf $result $benchmark >$stdout 2>$stderr || [ $? == 2 ]
[ -f $stdout ]; [ -s $stdout ]; rm $stdout
[ -f $stderr ]; [ ! -s $stderr ]; rm $stderr

# Should pass, because underscore creates unique suffix
$OSCAP xccdf generate fix --result-id _common --output $script $result 2> $stderr
# Checks that full result id is used in rule evaluation
# grep -Fq xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_common $script

# Should fail, because result with this id suffix does not exist
$OSCAP xccdf generate fix --result-id rare --output $script $result 2> $stderr || ret=$?
[ $ret -eq 1 ]
# Checks that last test does result in no match error
grep -Fq "No profile matching suffix \"rare\" was found" $stderr

# Multiple matches should result in failure
$OSCAP xccdf generate fix --result-id common --output $script $result 2> $stderr || ret=$?
[ $ret -eq 1 ]
# Checks that first test does result in multiple match error
grep -Fq "Multiple matches found" $stderr

[ -f $stderr ]; rm $stderr
rm $result
rm $script
