#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CPE Dict Module Test Suite.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>

. $builddir/tests/test_common.sh

# Test cases.

function test_api_cpe_dict_smoke {
    ./test_api_cpe_dict --smoke-test
}

function test_api_cpe_dict_remove_cpe {
    require "grep" || return 255
    ./test_api_cpe_dict --remove $srcdir/dict.xml "UTF-8" "cpe:/a:addsoft" | \
    grep -s "addsoft"
    return $([ $? -eq 1 ])
}

function test_api_cpe_dict_import_damaged_xml {
    ./test_api_cpe_dict --list-cpe-names $srcdir/dict-damaged.xml "UTF-8"
    return $([ $? -eq 2 ])
}

function test_api_cpe_dict_match_non_existing_cpe {
    ./test_api_cpe_dict --match $srcdir/dict.xml \
    "UTF-8" "cpe:/a:3com:3c16115-usNOT_IN_THE_DICTIONARY"
    return $([ $? -eq 1 ])
}

function test_api_cpe_dict_match_existing_cpe {
    require "grep" || return 255
    CPE_URIS=(`grep "cpe:" $srcdir/dict.xml | \
               sed 's/^.*cpe:/cpe:/g' | sed 's/".*$//g' | tr '\n' ' '`)    
    for URI in ${CPE_URIS[@]}; do
	./test_api_cpe_dict --match $srcdir/dict.xml "UTF-8" "$URI"
	[ ! $? -eq 0 ] && return 1
    done
    return 0 
}

function test_api_cpe_dict_export_xml {
    ./test_api_cpe_dict --export $srcdir/dict.xml "UTF-8" \
	dict.xml.out "UTF-8" && \
	$XMLDIFF $srcdir/dict.xml dict.xml.out
}

function test_api_cpe_dict_import_cp1250_xml {
    ./test_api_cpe_dict --list-cpe-names $srcdir/dict-cp1250-dos.xml "CP-1250"
}

function test_api_cpe_dict_import_utf8_xml {
    ./test_api_cpe_dict --list-cpe-names $srcdir/dict.xml "UTF8"
}

function test_api_cpe_dict_import_official_v22(){
	set -e -o pipefail
	local name="official-cpe-dictionary_v2.2.xml"
	local dict="$srcdir/$name"
	local out=$(mktemp -t $name.out.XXXXXX)

	$OSCAP info $dict 2> $out
	[ ! -s $out ]

	$OSCAP cpe validate $dict 2>&1 > $out
	[ ! -s $out ]

	./test_api_cpe_dict --list $dict "UTF-8" > $out
	grep 'National Vulnerability Database (NVD)' $out
	[ "`cat $out | wc -l`" == "1" ]

	./test_api_cpe_dict --list-cpe-names $dict "UTF-8" > $out
	grep 'cpe:/a:acronis:backup_%26_recovery_agent:10.0.11639' $out
	[ "`cat $out | wc -l`" == "8" ]

	rm -f dict.xml.out
	./test_api_cpe_dict --export $dict "UTF-8" dict.xml.out "UTF-8" 2>&1 > $out
	[ ! -s $out ]
	$XMLDIFF $dict dict.xml.out
	rm $out
}

function test_api_cpe_dict_import_official_v23(){
	set -e -o pipefail
	local name="official-cpe-dictionary_v2.3.xml"
	local dict="$srcdir/$name"
	local out=$(mktemp -t $name.out.XXXXXX)

	$OSCAP info $dict 2> $out
	[ ! -s $out ]

	$OSCAP cpe validate $dict 2>&1 > $out
	[ ! -s $out ]

	./test_api_cpe_dict --list $dict "UTF-8" > $out
	grep 'National Vulnerability Database (NVD)' $out
	[ "`cat $out | wc -l`" == "1" ]

	./test_api_cpe_dict --list-cpe-names $dict "UTF-8" > $out
	grep 'cpe:/h:hp:advancestack_10base-t_switching_hub_j3200a:a.03.07' $out
	[ "`cat $out | wc -l`" == "7" ]

	rm -f dict.xml.out
	./test_api_cpe_dict --export $dict "UTF-8" dict.xml.out "UTF-8" 2>&1 > $out
	[ ! -s $out ]
	$XMLDIFF $dict dict.xml.out
	rm $out
}

# Testing.

test_init "test_api_cpe_dict.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_api_cpe_dict_smoke" test_api_cpe_dict_smoke
    test_run "test_api_cpe_dict_remove_cpe" test_api_cpe_dict_remove_cpe
    test_run "test_api_cpe_dict_import_damaged_xml" \
        test_api_cpe_dict_import_damaged_xml
    test_run "test_api_cpe_dict_match_non_existing_cpe" \
        test_api_cpe_dict_match_non_existing_cpe   
    test_run "test_api_cpe_dict_match_existing_cpe" \
        test_api_cpe_dict_match_existing_cpe
    test_run "test_api_cpe_dict_export_xml"  test_api_cpe_dict_export_xml
    #test_run "test_api_cpe_dict_import_cp1250_xml" \
    #    test_api_cpe_dict_import_cp1250_xml   
    test_run "test_api_cpe_dict_import_utf8_xml" test_api_cpe_dict_import_utf8_xml
    test_run "test_api_cpe_dict_import_official_v22" test_api_cpe_dict_import_official_v22
    test_run "test_api_cpe_dict_import_official_v23" test_api_cpe_dict_import_official_v23
fi

test_exit
