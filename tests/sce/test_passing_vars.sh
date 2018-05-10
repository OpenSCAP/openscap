#!/usr/bin/env bash

# Author:
#	Michal Srubar <msrubar@redhat.com>

# Test whether the value of set with <refine-value> or <set-value> is passed to
# the SCE script correctly or not.

set -e -o pipefail

. "$builddir/tests/test_common.sh"

# Test Cases.
function test_passing_values {

  local DEFFILE=${srcdir}/$1
  local RESFILE=$1.results

  [ -f $RESFILE ] && rm -f $RESFILE

  $OSCAP xccdf eval --results "$RESFILE" --profile "default" "$DEFFILE"

  # ensure that value set with <refine-value> was passed correctly
  grep "VALUE_1=fail" "$RESFILE"
  # ensure that value set with <set-value> was passed correctly
  grep "VALUE_2=my_pass_val" "$RESFILE"
  rm $RESFILE
}

# Testing.
test_init "test_passing_values.log"

test_run "sce" test_passing_values test_passing_vars_xccdf.xml

test_exit

