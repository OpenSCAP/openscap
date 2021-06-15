#!/usr/bin/env bash

# Author:
#   Jan Černý <jcerny@redhat.com>

set -e -o pipefail
set -x

. $builddir/tests/test_common.sh
	
PROFILE="xccdf_com.example.www_profile_test_remote_res"
result=$(mktemp)
stderr=$(mktemp)
tmpdir=$(mktemp -d)
cp "${srcdir}/ds_use_local_remote_resources/remote_content_1.3.ds.xml" "$tmpdir"
cp "${srcdir}/ds_use_local_remote_resources/remote.oval.xml" "$tmpdir"
pushd "$tmpdir"

$OSCAP xccdf eval --use-local-file --profile "$PROFILE" --results "$result" "remote_content_1.3.ds.xml" 2>"$stderr" || ret=$?
[ "$ret" = 2 ]

grep -q "WARNING: Datastream component 'scap_org.open-scap_cref_remote.oval.xml' points out to the remote 'https://www.example.com/security/data/oval/remote.oval.xml'. Use '--fetch-remote-resources' option to download it." "$stderr" && false
grep -q "WARNING: Skipping 'https://www.example.com/security/data/oval/remote.oval.xml' file which is referenced from datastream" "$stderr" && false

assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_test-pass"]/result[text()="pass"]'
# the remote_res rule is a multicheck with 2 oval definitions so it's twice here
assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_test-remote_res"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_test-remote_res"]/result[text()="fail"]'
assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_test-pass2"]/result[text()="pass"]'

popd
rm -f "$result" "$stderr"
rm -rf "$tmpdir"


# test the same without --use-local-file to make sure the $tmpdir/remote.oval.xml isn't loaded by oscap

result=$(mktemp)
stderr=$(mktemp)
tmpdir=$(mktemp -d)
cp "${srcdir}/ds_use_local_remote_resources/remote_content_1.3.ds.xml" "$tmpdir"
cp "${srcdir}/ds_use_local_remote_resources/remote.oval.xml" "$tmpdir"
pushd "$tmpdir"

$OSCAP xccdf eval --profile "$PROFILE" --results "$result" "remote_content_1.3.ds.xml" 2>"$stderr" || ret=$?
[ "$ret" = 2 ]

grep -q "WARNING: Datastream component 'scap_org.open-scap_cref_remote.oval.xml' points out to the remote 'https://www.example.com/security/data/oval/remote.oval.xml'. Use '--fetch-remote-resources' option to download it." "$stderr"
grep -q "WARNING: Skipping 'https://www.example.com/security/data/oval/remote.oval.xml' file which is referenced from datastream" "$stderr"

assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_test-pass"]/result[text()="pass"]'
assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_test-remote_res"]/result[text()="notchecked"]'
assert_exists 1 '//rule-result[@idref="xccdf_com.example.www_rule_test-pass2"]/result[text()="pass"]'

popd
rm -f "$result" "$stderr"
rm -rf "$tmpdir"