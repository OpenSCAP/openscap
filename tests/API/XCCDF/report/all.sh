#!/usr/bin/env bash

# Copyright 2012 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. ../../../test_common.sh

function test_generate_report {
    local INPUT=$srcdir/$1
    local EXPECTED_CONTENT=$2

    local GENERATED_CONTENT=$($OSCAP xccdf generate report "$INPUT")
    if [ "$?" != "0" ]; then
        return 1
    fi

    echo "$GENERATED_CONTENT" | grep "$EXPECTED_CONTENT"
    if [ "$?" == "0" ]; then
        return 0
    fi

    echo "Generated content does not contain '$EXPECTED_CONTENT'!"
    echo "Generated content:"
    echo "$GENERATED_CONTENT"

    return 1
}

function test_generate_report_custom {
    local INPUT=$srcdir/$1
    local EXPECTED_CONTENT=$2

    local GENERATED_CONTENT=$($OSCAP xccdf generate custom --stylesheet $OSCAP_XSLT_PATH/xccdf-report.xsl "$INPUT")
    if [ "$?" != "0" ]; then
        return 1
    fi

    echo "$GENERATED_CONTENT" | grep "$EXPECTED_CONTENT"
    if [ "$?" == "0" ]; then
        return 0
    fi

    echo "Generated content does not contain '$EXPECTED_CONTENT'!"
    echo "Generated content:"
    echo "$GENERATED_CONTENT"

    return 1
}

# Testing.

test_init "test_api_xccdf_report.log"

test_run "test_api_xccdf_report_xccdf11" test_generate_report results-xccdf11.xml xccdf_moc.elpmaxe.www_rule_1
test_run "test_api_xccdf_report_xccdf12" test_generate_report results-xccdf12.xml xccdf_moc.elpmaxe.www_rule_1
# This does not work because the xccdf-report.xsl is made for XCCDF 1.2, the namespace workaround
# (see xccdf_ns_xslt_workaround(..) for details) is not used when using custom stylesheets.
#test_run "test_api_xccdf_report_custom_xccdf11" test_generate_report_custom results-xccdf11.xml xccdf_moc.elpmaxe.www_rule_1
test_run "test_api_xccdf_report_custom_xccdf12" test_generate_report_custom results-xccdf12.xml xccdf_moc.elpmaxe.www_rule_1
test_run "test_api_xccdf_report_idents" test_generate_report results-idents-refs.xml identidentident
test_run "test_api_xccdf_report_refs" test_generate_report results-idents-refs.xml referencereferencereference
test_run "test_api_xccdf_report_no_title" test_generate_report results-xccdf12.xml "ID: xccdf_moc.elpmaxe.www_rule_1"
test_run "test_api_xccdf_report_title" test_generate_report results-title.xml "RULETITLE"

test_exit
