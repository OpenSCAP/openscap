#!/usr/bin/env bash

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CPE Dict Module Test Suite.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>

. ${srcdir}/test_common.sh

# Setup.
function test_cpedict_setup {
    local ret_val=0

    CPE_URIS=(`grep "cpe:" ${srcdir}/CPE/dict.xml | sed 's/^.*cpe:/cpe:/g' \
	| sed 's/".*$//g' | tr '\n' ' '`)
    
    [ ${#CPE_URIS[@]} -eq 350 ] || ret_val=1
    
    return $ret_val
}

# Test Cases.
function test_cpedict_smoke {
    local ret_val=0;

    ${srcdir}/test_cpedict --smoke-test
    ret_val=$?

    return $ret_val
}

function test_cpedict_import_utf8_xml {
    local ret_val=0;

    ${srcdir}/test_cpedict --list-cpe-names ${srcdir}/CPE/dict.xml "UTF-8" > \
	test_cpedict_tc02_01.out
    ret_val=$?

    if [ $ret_val -eq 0 ]; then
	grep "<cpe-item" ${srcdir}/CPE/dict.xml | \
	    sed -e 's/^.*name="\(.*\)".*>/\1/' -e 's/" .*$//' > test_cpedict_tc02_00.out
	cmp test_cpedict_tc02_00.out test_cpedict_tc02_01.out >&2
	diff test_cpedict_tc02_00.out test_cpedict_tc02_01.out >&2
	ret_val=$?
    fi

    return $ret_val
}

function test_cpedict_import_cp1250_xml {
    local ret_val=0;

    ${srcdir}/test_cpedict --list-cpe-names ${srcdir}/CPE/dict-cp1250-dos.xml "X" >&2
    ret_val=$?

    return $ret_val
}

function test_cpedict_import_damaged_xml {
    local ret_val=0;

    ${srcdir}/test_cpedict --list-cpe-names ${srcdir}/CPE/dict-damaged.xml "UTF-8" >&2
    [ $? -eq 0 ] && ret_val=1

    return $ret_val
}

function test_cpedict_match_existing_cpe {
    local ret_val=0;

    for URI in ${CPE_URIS[@]}; do
	${srcdir}/test_cpedict --match ${srcdir}/CPE/dict.xml "UTF-8" "$URI" >&2
	ret_val=$[$ret_val + $?]
    done

    return $ret_val
}

function test_cpedict_match_non_existing_cpe {
    local ret_val=0;

    ${srcdir}/test_cpedict --match ${srcdir}/CPE/dict.xml "UTF-8" \
	"cpe:/a:3com:3c16115-usNOT_IN_THE_DICTIONARY" >&2
    [ $? -eq 0 ] && ret_val=1

    return $ret_val
}

function test_cpedict_export_xml {
    local ret_val=0;

    ${srcdir}/test_cpedict --export ${srcdir}/CPE/dict.xml "UTF-8" \
	${srcdir}/dict.xml.exported "UTF-8"

    if [ -e ${srcdir}/dict.xml.exported ]; then
	xml_cmp ${srcdir}/CPE/dict.xml ${srcdir}/dict.xml.exported >&2 
	ret_val=$?
    else
	ret_val=1
    fi

    return $ret_val
}

function test_cpedict_remove_cpe {
    local ret_val=0;

    ${srcdir}/test_cpedict --remove ${srcdir}/CPE/dict.xml "UTF-8" "cpe:/a:addsoft" \
	| grep -q "addsoft" >&2

    [ $? -eq 0 ] && ret_val=1

    return $ret_val
}

# Cleanup.
function test_cpedict_cleanup { 
    local ret_val=0;

    rm ${srcdir}/dict.xml.exported

    ret_val=$?

    return $ret_val
}

# TESTING.

echo ""
echo "--------------------------------------------------"

result=0
log=${srcdir}/test_cpedict.log

exec 2>$log

test_cpedict_setup    
ret_val=$? 
report_result "test_cpedict_setup" $ret_val
result=$[$result+$ret_val]

test_cpedict_smoke
ret_val=$?
report_result "test_cpedict_smoke" $ret_val 
result=$[$result+$ret_val]   

test_cpedict_import_utf8_xml 
ret_val=$? 
report_result "test_cpedict_import_utf8_xml" $ret_val 
result=$[$result+$ret_val]   

# test_cpedict_import_cp1250_xml 
# ret_val=$? 
# report_result "test_cpedict_import_cp1250_xml" $ret_val 
# result=$[$result+$ret_val]   

test_cpedict_import_damaged_xml
ret_val=$? 
report_result "test_cpedict_import_damaged_xml" $ret_val  
result=$[$result+$ret_val]   

test_cpedict_match_existing_cpe
ret_val=$? 
report_result "test_cpedict_match_existing_cpe" $ret_val  
result=$[$result+$ret_val]   

test_cpedict_match_non_existing_cpe
ret_val=$? 
report_result "test_cpedict_match_non_existing_cpe" $ret_val  
result=$[$result+$ret_val]   

test_cpedict_export_xml
ret_val=$? 
report_result "test_cpedict_export_xml" $ret_val  
result=$[$result+$ret_val]   

test_cpedict_remove_cpe
ret_val=$? 
report_result "test_cpedict_remove_cpe" $ret_val  
result=$[$result+$ret_val]   

test_cpedict_cleanup  
ret_val=$? 
report_result "test_cpedict_cleanup" $ret_val 
result=$[$result+$ret_val]

echo "--------------------------------------------------"
echo "See ${log} (in tests dir)"

exit $result
