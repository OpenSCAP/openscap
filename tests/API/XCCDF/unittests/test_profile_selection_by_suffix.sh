#!/bin/bash

set -e
set -o pipefail

name=$(basename $0 .sh)
stderr=$(mktemp -t ${name}.out.XXXXXX)
ret=0

# Multiple matches should result in failure
$OSCAP xccdf eval --profile common $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 1 ]

# Should pass, because underscore creates unique suffix
$OSCAP xccdf eval --profile _common $srcdir/${name}.xccdf.xml 2> $stderr

# Should fail, because a profile with this suffix does not exist
$OSCAP xccdf eval --profile xcommon $srcdir/${name}.xccdf.xml 2> $stderr || ret=$?
[ $ret -eq 1 ]
 
