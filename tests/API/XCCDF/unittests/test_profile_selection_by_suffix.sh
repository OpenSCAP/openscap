#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
result=$(mktemp -t ${name}.out.XXXXXX)
benchmark=$srcdir/${name}.xccdf.xml
tailoring=$srcdir/${name}_tailoring.xccdf.xml
echo "Stderr file = $stderr"
echo "Result file = $result"
ret=0

# Multiple matches should result in failure
$OSCAP xccdf eval --profile common $benchmark 2> $stderr || ret=$?
[ $ret -eq 1 ]
# Checks that first test does result in multiple match error
grep -Fq "Multiple matches found" $stderr

# Should pass, because underscore creates unique suffix
$OSCAP xccdf eval --profile _common --results $result $benchmark 2> $stderr
# Checks that full profile name is used in rule evaluation
grep -Fq xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_common $result

# Should fail, because a profile with this suffix does not exist
$OSCAP xccdf eval --profile xcommon $benchmark 2> $stderr || ret=$?
[ $ret -eq 1 ]
# Checks that last test does result in no match error
grep -Fq "No profile matching suffix \"xcommon\" was found" $stderr


# Should pass, because only one match is in tailoring file, which takes precedence
$OSCAP xccdf eval --tailoring-file $tailoring --profile common $benchmark 2> $stderr
# Checks that full profile name is used in rule evaluation
grep -Fq xccdf_org.open-scap_testresult_xccdf_moc.elpmaxe.www_profile_common $result

# Should fail, because there will be multiple suffix matches
$OSCAP xccdf eval --tailoring-file $tailoring --profile her $benchmark 2> $stderr || ret=$?
[ $ret -eq 1 ]
# Checks that multiple match error does result
grep -Fq "Multiple matches found" $stderr

# Should pass, because underscore creates unique suffix
$OSCAP xccdf eval --tailoring-file $tailoring --profile _her --results $result $benchmark 2> $stderr
# Checks that full profile name is used in rule evaluation
grep -Fq xccdf_org.open-scap_testresult_xccdf_org.ssgproject.content_profile_her $result

# Should fail, because a profile with this suffix does not exist
$OSCAP xccdf eval --tailoring-file $tailoring --profile another $benchmark 2> $stderr || ret=$?
[ $ret -eq 1 ]
# Checks that last test does result in no match error
grep -Fq "No profile matching suffix \"another\" was found" $stderr

[ -f $stderr ]; rm $stderr
rm $result
