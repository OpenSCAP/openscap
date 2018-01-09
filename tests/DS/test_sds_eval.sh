#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

set -e -o pipefail

. $builddir/tests/test_common.sh
. $srcdir/test_ds_common.sh

# Test Cases.

function test_eval {
    local stderr=$(mktemp -t ${name}.out.XXXXXX)
    $OSCAP xccdf eval "${srcdir}/$1" 2> $stderr
    diff /dev/null $stderr; rm $stderr
}

function test_eval_cpe {
    local stdout=$(mktemp -t ${name}.out.XXXXXX)
    local stderr=$(mktemp -t ${name}.err.XXXXXX)
    local ret=0

    $OSCAP xccdf eval --progress "${srcdir}/$1" 1> $stdout 2> $stderr || ret=$?
    grep -q "rule_applicable_pass:pass" $stdout
    grep -q "rule_applicable_fail:fail" $stdout
    grep -q "rule_notapplicable:notapplicable" $stdout
    diff /dev/null $stderr
    rm $stdout $stderr
}

function test_invalid_eval {
    local ret=0
    $OSCAP xccdf eval "${srcdir}/$1" || ret=$?
    return $([ $ret -eq 1 ])
}

function test_invalid_oval_eval {
    local ret=0
    $OSCAP oval eval "${srcdir}/$1" || ret=$?
    return $([ $ret -eq 1 ])
}

function test_eval_id {

    OUT=$($OSCAP xccdf eval --datastream-id $2 --xccdf-id $3 "${srcdir}/$1")
    local RET=$?

    if [ $RET -ne 0 ]; then
        return 1
    fi

    echo "$OUT" | grep $4 > /dev/null
}

function test_eval_benchmark_id {

    OUT=$($OSCAP xccdf eval --benchmark-id $2 "${srcdir}/$1")
    local RET=$?

    if [ $RET -ne 0 ]; then
        return 1
    fi

    echo "$OUT" | grep $3 > /dev/null
}

function test_eval_complex()
{
	local name=${FUNCNAME}
	local arf=$(mktemp -t ${name}.arf.XXXXXX)
	local stderr=$(mktemp -t ${name}.err.XXXXXX)
	local stdout=$(mktemp -t ${name}.out.XXXXXX)

	$OSCAP xccdf eval \
		--results-arf $arf \
		--datastream-id scap_org.open-scap_datastream_tst2 \
		--xccdf-id scap_org.open-scap_cref_second-xccdf.xml2 \
		--profile xccdf_moc.elpmaxe.www_profile_2 \
		$srcdir/eval_xccdf_id/sds-complex.xml 2> $stderr > $stdout

	# Ensure the sanity of the output.
	[ -f $stderr ]; [ ! -s $stderr ]
	[ "`grep ^Rule $stdout | wc -l`" == "1" ]
	grep ^Rule $stdout | grep xccdf_moc.elpmaxe.www_rule_secon
	rm $stdout

	# Ensure basic correctness of the ARF
	$OSCAP ds rds-validate $arf 2>&1 > $stderr
	[ -f $srderr ]; [ ! -s $stderr ]; rm $stderr
	assert_correct_xlinks $arf

	# Ensure that results are there
	local result="$arf"
	assert_exists 1 '//rule-result'
	assert_exists 1 '//rule-result[@idref="xccdf_moc.elpmaxe.www_rule_second"]'
	assert_exists 1 '//rule-result/result'
	assert_exists 1 '//rule-result/result[text()="pass"]'
	rm $arf
}

function test_oval_eval {

    $OSCAP oval eval "${srcdir}/$1"
}

function test_oval_eval_id {

    OUT=$($OSCAP oval eval --datastream-id $2 --oval-id $3 "${srcdir}/$1")
    local RET=$?

    if [ $RET -ne 0 ]; then
        return 1
    fi
    echo "out: $OUT"

    echo "$OUT" | grep $4 > /dev/null
}

function test_sds_external_xccdf {
    local SDS_FILE="${srcdir}/$2"
    local XCCDF="$3"
    local PROFILE="$4"
    local result="${1}-${PROFILE}.xml"

    $OSCAP xccdf eval --xccdf-id "$XCCDF" --profile "$PROFILE" --results "$result" "$SDS_FILE"

    assert_exists 1 '//rule-result/result[text()="pass"]'

    rm -f "$result"
}

function test_sds_tailoring {
	local SDS_FILE="${srcdir}/$2"
	local DATASTREAM_ID="$3"
	local TAILORING_ID="$4"
	local PROFILE="$5"
	local result=$(mktemp)

	$OSCAP info "$SDS_FILE"

	$OSCAP xccdf eval --datastream-id "$DATASTREAM_ID" --tailoring-id "$TAILORING_ID" --profile "$PROFILE" --results "$result" "$SDS_FILE"

	assert_exists 2 '//Rule'
	assert_exists 1 '//Rule[@id="xccdf_com.example_rule_1" and @selected="true"]'
	assert_exists 1 '//Rule[@id="xccdf_com.example_rule_2" and @selected="false"]'
	assert_exists 2 '//rule-result'
	assert_exists 1 '//rule-result[@idref="xccdf_com.example_rule_1"]'
	assert_exists 1 '//rule-result[@idref="xccdf_com.example_rule_2"]'
	assert_exists 1 '//rule-result[@idref="xccdf_com.example_rule_1"]/result[text()="notselected"]'
	assert_exists 1 '//rule-result[@idref="xccdf_com.example_rule_2"]/result[text()="pass"]'

	rm -f "$result"
}

# Testing.
test_init "test_sds_eval.log"

test_run "sds_external_xccdf" test_sds_external_xccdf sds_external_xccdf sds_external_xccdf/sds.ds.xml scap_org.open-scap_cref_xccdf.xml xccdf_external_profile_datastream_1
test_run "sds_external_xccdf" test_sds_external_xccdf sds_external_xccdf sds_external_xccdf/sds.ds.xml scap_org.open-scap_cref_xccdf-file.xml xccdf_external_profile_file_1

test_run "sds_tailoring" test_sds_tailoring sds_tailoring sds_tailoring/sds.ds.xml scap_com.example_datastream_with_tailoring xccdf_com.example_cref_tailoring_01 xccdf_com.example_profile_tailoring

test_run "eval_simple" test_eval eval_simple/sds.xml
test_run "cpe_in_ds" test_eval cpe_in_ds/sds.xml
test_run "eval_invalid" test_invalid_eval eval_invalid/sds.xml
test_run "eval_invalid_oval" test_invalid_oval_eval eval_invalid/sds-oval.xml
test_run "eval_xccdf_id1" test_eval_id eval_xccdf_id/sds.xml scap_org.open-scap_datastream_tst scap_org.open-scap_cref_first-xccdf.xml first
test_run "eval_xccdf_id2" test_eval_id eval_xccdf_id/sds.xml scap_org.open-scap_datastream_tst scap_org.open-scap_cref_second-xccdf.xml second
test_run "eval_benchmark_id1" test_eval_benchmark_id eval_xccdf_id/sds.xml xccdf_moc.elpmaxe.www_benchmark_first first
test_run "eval_benchmark_id2" test_eval_benchmark_id eval_xccdf_id/sds.xml xccdf_moc.elpmaxe.www_benchmark_second second
test_run "eval_benchmark_id_conflict" test_eval_benchmark_id eval_benchmark_id_conflict/sds.xml xccdf_moc.elpmaxe.www_benchmark_first first
test_run "eval_just_oval" test_oval_eval eval_just_oval/sds.xml
test_run "eval_oval_id1" test_oval_eval_id eval_oval_id/sds.xml scap_org.open-scap_datastream_just_oval scap_org.open-scap_cref_scap-oval1.xml "oval:x:def:1"
test_run "eval_oval_id2" test_oval_eval_id eval_oval_id/sds.xml scap_org.open-scap_datastream_just_oval scap_org.open-scap_cref_scap-oval2.xml "oval:x:def:2"
test_run "eval_cpe" test_eval_cpe eval_cpe/sds.xml

test_run "test_eval_complex" test_eval_complex

test_exit
