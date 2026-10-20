#!/usr/bin/env bash
. $builddir/tests/test_common.sh

set -e
set -o pipefail

stdout=$(mktemp)

$OSCAP xccdf eval --progress --profile "xccdf_com.example.www_profile_test" "$srcdir/test_no_newline_between_select_elements.xml" > "$stdout" || true

# test if both rules selected in the profile were evaluated
grep -q "xccdf_com.example.www_rule_first:pass" "$stdout"
grep -q "xccdf_com.example.www_rule_second:fail" "$stdout"

rm -f "$stdout"
