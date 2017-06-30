#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
script=$(mktemp -t ${name}.sh.XXXXXX)
result=$srcdir/${name}.xml
echo "Stderr file = $stderr"
echo "Result file = $result"
ret=0

# Should pass, unique TestResult ID suffix
$OSCAP xccdf generate fix --result-id standard --output $script $result 2> $stderr
# Checks that full result id is used in rule evaluation
# grep -Fq xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_standard $script

# Should pass, because underscore creates unique suffix
$OSCAP xccdf generate fix --result-id _common --output $script $result 2> $stderr
# Checks that full result id is used in rule evaluation
# Will work with script header generation 
# grep -Fq xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_common $script

# Should fail, because result with this id suffix does not exist
$OSCAP xccdf generate fix --result-id rare --output $script $result 2> $stderr || ret=$?
[ $ret -eq 1 ]
# Checks that last test does result in no match error
# Will work with script header generation 
grep -Fq "Could not find TestResult/@id=\"rare\"" $stderr

# Multiple matches should result in failure
$OSCAP xccdf generate fix --result-id common --output $script $result 2> $stderr || ret=$?
[ $ret -eq 1 ]
# Checks that first test does result in multiple match error
grep -Fq "Multiple matches found" $stderr

[ -f $stderr ]; rm $stderr
rm $script
