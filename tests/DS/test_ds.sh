#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

set -e -o pipefail

. ../test_common.sh

# Test Cases.

function assert_correct_xlinks()
{
	local DS=$1
	local stderr=$(mktemp)
	$OSCAP info $DS 2> $stderr
	diff $stderr /dev/null
	rm $stderr

	# First of all make sure that there is at least one ds:component-ref.
	[ "$($XPATH $DS 'count(//*[local-name()="component-ref"])')" != "0" ]
	# We want to catch cases when this element has different namespace.
	local ns=$($XPATH $DS 'name(//*[local-name()="component-ref"][1])' | sed 's/:.*$/:/')
	[ "$ns" != "component-ref" ] || ns=""
	# Ensure there is at least some xlink.
	[ "`$XPATH $DS \"count(//${ns}component-ref/@xlink:href)\"`" != "0" ]
	# This asserts that there is none component-ref/xlink:href broken.
	# Previously, we have seen datastreams with broken xlinks (see trac#286).
	[ "`$XPATH $DS  \"count(//${ns}component-ref[substring(@xlink:href, 2, 10000) != (//${ns}component/@${ns}id | //${ns}extended-component/@${ns}id)])\"`" == "0" ]
}

sds_add_multiple_twice(){
	local DIR="${srcdir}/sds_multiple_oval"
	local XCCDF_FILE="multiple-oval-xccdf.xml"
	local DS_TARGET_DIR="$(mktemp -d)"
	local DS_FILE="$DS_TARGET_DIR/sds.xml"
	local stderr=$(mktemp -t sds_add.out.XXXXXX)

	# Create DS from scratch
	pushd "$DIR"
	$OSCAP ds sds-compose "$XCCDF_FILE" "$DS_FILE" 2>&1 > $stderr
	diff $stderr /dev/null
	popd

	# Add the very same XCCDF file again with two OVAL files
	local ADD_DIR="$(mktemp -d)"
	cp ${DIR}/*.xml ${ADD_DIR}
	chmod u+w ${ADD_DIR}/* # distcheck shall be able to unlink these files (without --force)
	local XCCDF2="$ADD_DIR/$XCCDF_FILE"
	pushd ${ADD_DIR}
	$OSCAP ds sds-add "$XCCDF2" "$DS_FILE" 2>&1 > $stderr
	local ifiles=$(ls *.xml)
	popd
	diff $stderr /dev/null
	rm $XCCDF2 ${ADD_DIR}/*-oval.xml
	rm -f ${ADD_DIR}/oscap_debug.log.*
	rmdir ${ADD_DIR}

	$OSCAP ds sds-validate "$DS_FILE" 2>&1 > $stderr
	diff $stderr /dev/null
	assert_correct_xlinks "$DS_FILE"
	$OSCAP info "$DS_FILE" 2> $stderr
	diff $stderr /dev/null

	local result=$DS_FILE
	assert_exists 1 '/ds:data-stream-collection/ds:data-stream'
	assert_exists 2 '/ds:data-stream-collection/ds:data-stream/*'
	assert_exists 1 '/ds:data-stream-collection/ds:data-stream/ds:checklists'
	assert_exists 2 '/ds:data-stream-collection/ds:data-stream/ds:checklists/*'
	assert_exists 2 '/ds:data-stream-collection/ds:data-stream/ds:checklists/ds:component-ref'
	assert_exists 1 '/ds:data-stream-collection/ds:data-stream/ds:checks'
	assert_exists 4 '/ds:data-stream-collection/ds:data-stream/ds:checks/*'
	assert_exists 4 '/ds:data-stream-collection/ds:data-stream/ds:checks/ds:component-ref'
	assert_exists 6 '/ds:data-stream-collection/ds:component'
	assert_exists 4 '/ds:data-stream-collection/ds:component/oval_definitions'
	assert_exists 2 '/ds:data-stream-collection/ds:component/xccdf:Benchmark'

	# split the SDS and verify the content
	pushd "$DS_TARGET_DIR"
	$OSCAP ds sds-split "`basename $DS_FILE`" "$DS_TARGET_DIR"
	[ ! -f multiple-oval-xccdf.xml ]
	mv scap_org.open-scap_cref_multiple-oval-xccdf.xml multiple-oval-xccdf.xml
	popd
	local f
	for f in second-oval.xml first-oval.xml multiple-oval-xccdf.xml; do
		$OSCAP info ${DS_TARGET_DIR}/$f 2> $stderr
		diff $stderr /dev/null
		diff ${DS_TARGET_DIR}/$f ${DIR}/$f
		rm ${DS_TARGET_DIR}/$f
	done
	rm $DS_FILE
	rm -f $DS_TARGET_DIR/oscap_debug.log.*
	rmdir $DS_TARGET_DIR
	rm $stderr
}

function test_sds {

    local ret_val=0;

    local DIR="${srcdir}/$1"
    local XCCDF_FILE="$2"
    local SKIP_DIFF="$3"
    local DS_TARGET_DIR="`mktemp -d`"
    local DS_FILE="$DS_TARGET_DIR/sds.xml"

    pushd "$DIR"

    $OSCAP ds sds-compose "$XCCDF_FILE" "$DS_FILE"

    assert_correct_xlinks $DS_FILE
    popd

    pushd "$DS_TARGET_DIR"

    $OSCAP ds sds-split "`basename $DS_FILE`" "$DS_TARGET_DIR"

    rm "$DS_FILE"

    # get rid of filler prefix to make the diff work
    for file in scap_org.open-scap_cref_*;
    do
        mv "$file" "${file#scap_org.open-scap_cref_}"
    done

    popd

    if [ "$SKIP_DIFF" != "1" ]; then
        if ! diff --exclude "oscap_debug.log.*" "$DIR" "$DS_TARGET_DIR"; then
            echo "The files are different after going through source data stream!"
            echo

            ret_val=1
        fi
    fi

    rm -r "$DS_TARGET_DIR"

    return "$ret_val"
}

function test_eval {

    $OSCAP xccdf eval "${srcdir}/$1"
}

function test_generate_fix {

    $OSCAP xccdf generate fix "${srcdir}/$1"
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

function test_rds
{
    local ret_val=0;

    local SDS_FILE="${srcdir}/$1"
    local XCCDF_RESULT_FILE="${srcdir}/$2"
    local OVAL_RESULT_FILE="${srcdir}/$3"
    local DS_TARGET_DIR="`mktemp -d`"
    local DS_FILE="$DS_TARGET_DIR/rds.xml"

    $OSCAP ds rds-create "$SDS_FILE" "$DS_FILE" "$XCCDF_RESULT_FILE" "$OVAL_RESULT_FILE"

    if [ $? -ne 0 ]; then
        ret_val=1
    fi

    assert_correct_xlinks $DS_FILE

    #pushd "$DS_TARGET_DIR"
    #$OSCAP ds sds_split "`basename $DS_FILE`" "$DS_TARGET_DIR"
    #rm sds.xml
    #popd

    rm -r "$DS_TARGET_DIR"

    return "$ret_val"
}

function test_rds_index
{
    local ret_val=0;

    local RDS_FILE="${srcdir}/$1"
    local ASSETS="$2"
    local REPORTS="$3"
    local REQUESTS="$4"

    INDEX=$($OSCAP info "$RDS_FILE")

    for asset in "$ASSETS"; do
        if ! echo $INDEX | grep --quiet "$asset"; then
            ret_val=1
            echo "Asset $asset expected in index"
        fi
    done

    for report in "$REPORTS"; do
        if ! echo $INDEX | grep --quiet "$report"; then
            ret_val=1
            echo "Report $report expected in index"
        fi
    done

    for requests in "$REQUESTS"; do
        if ! echo $INDEX | grep --quiet "$request"; then
            ret_val=1
            echo "Report request $request expected in index"
        fi
    done

    return "$ret_val"
}

function test_rds_split {

    local ret_val=0;

    local DIR="${srcdir}/$1"
    local SDS_FILE="$2"
    local REPORT_FILE="$3"
    local SKIP_DIFF="$4"
    local DS_TARGET_DIR="`mktemp -d`"
    local DS_FILE="$DS_TARGET_DIR/arf.xml"

    pushd "$DIR"

    $OSCAP ds rds-create "$SDS_FILE" "$DS_FILE" "$REPORT_FILE"

    assert_correct_xlinks $DS_FILE
    popd

    pushd "$DS_TARGET_DIR"

    $OSCAP ds rds-split "`basename $DS_FILE`" "$DS_TARGET_DIR"

    rm "$DS_FILE"
    popd

    if [ "$SKIP_DIFF" != "1" ]; then
        if ! diff --exclude "oscap_debug.log.*" "$DIR" "$DS_TARGET_DIR"; then
            echo "The files are different after going through result data stream!"
            echo

            ret_val=1
        fi
    fi

    rm -r "$DS_TARGET_DIR"

    return "$ret_val"
}

# Testing.
test_init "test_ds.log"

test_run "sds_simple" test_sds sds_simple scap-fedora14-xccdf.xml 0
test_run "sds_multiple_oval" test_sds sds_multiple_oval multiple-oval-xccdf.xml 0
test_run "sds_missing_oval-prepare" [ ! -f sds_missing_oval/second-oval.xml ]
test_run "sds_missing_oval" test_sds sds_missing_oval multiple-oval-xccdf.xml 0
test_run "sds_subdir" test_sds sds_subdir subdir/scap-fedora14-xccdf.xml 1
test_run "sds_extended_component" test_sds sds_extended_component fake-check-xccdf.xml 0
test_run "sds_extended_component_plain_text" test_sds sds_extended_component_plain_text fake-check-xccdf.xml 0
test_run "sds_extended_component_plain_text_entities" test_sds sds_extended_component_plain_text_entities fake-check-xccdf.xml 0
test_run "sds_extended_component_plain_text_whitespace" test_sds sds_extended_component_plain_text_whitespace fake-check-xccdf.xml 0

test_run "eval_simple" test_eval eval_simple/sds.xml
test_run "generate_fix_simple" test_generate_fix eval_simple/sds.xml
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
test_run "eval_cpe" test_eval eval_cpe/sds.xml
test_run "generate_fix_cpe" test_generate_fix eval_cpe/sds.xml

test_run "rds_simple" test_rds rds_simple/sds.xml rds_simple/results-xccdf.xml rds_simple/results-oval.xml
test_run "rds_testresult" test_rds rds_testresult/sds.xml rds_testresult/results-xccdf.xml rds_testresult/results-oval.xml
test_run "rds_index_simple" test_rds_index rds_index_simple/arf.xml "asset0 asset1" "report0" "collection0"
test_run "rds_split_simple" test_rds_split rds_split_simple report-request.xml report.xml 0

test_run "test_eval_complex" test_eval_complex
test_run "sds_add_multiple_oval_twice_in_row" sds_add_multiple_twice

test_exit

