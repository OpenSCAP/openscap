#!/bin/sh

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CPE Dict Module Test Suite.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>
#
# TODO: 
#      complete testing

. ${srcdir}/test_common.sh

# Setup.
function test_cpedict_setup {
    local ret_val=0

    CPE_URIS=(`grep "cpe:" ${srcdir}/CPE/dict.xml | sed 's/^.*cpe:/cpe:/g' | sed 's/".*$//g' | tr '\n' ' '`)

    return $ret_val
}

# Test Cases.
function test_cpedict_tc01 {
    local ret_val=0;

    ./test_cpedict --sanity-check
    ret_val=$?

    return $ret_val
}

function test_cpedict_tc02 {
    local ret_val=0;

    for URI in ${CPE_URIS[@]}; do
	./test_cpedict --match ${srcdir}/CPE/dict.xml "UTF-8" "$URI"
	ret_val=$[$ret_val + $?]
    done

    return $ret_val
}

function test_cpedict_tc03 {
    local ret_val=0;

    ./test_cpedict --match ${srcdir}/CPE/dict.xml "UTF-8" "cpe:/a:3com:3c16115-usNOT_IN_THE_DICTIONARY"
    [ $? -eq 0 ] && ret_val=1

    return $ret_val
}

function test_cpedict_tc04 {
    local ret_val=0;

    ./test_cpedict --export ${srcdir}/CPE/dict.xml "UTF-8" dict.xml.exported "UTF-8"

    if [ -e dict.xml.exported ]; then
	xml_cmp ${srcdir}/CPE/dict.xml dict.xml.exported
	ret_val=$?
    else
	ret_val=1
    fi

    return $ret_val
}

function test_cpedict_tc05 {
    local ret_val=0;

    ./test_cpedict --remove ${srcdir}/CPE/dict.xml "UTF-8" 'cpe:/a:addsoft' | grep 'addsoft'

	if [ "$?" == "1" ]; then
		# grep did not find addsoft
		ret_val=0
	else
		ret_val=1
	fi

    return $ret_val
}

# Cleanup.
function test_cpedict_cleanup { 
    local ret_val=0;

    rm dict.xml.exported
    
    return $ret_val
}

# TESTING.

echo "------------------------------------------"

result=0
log=test_cpedict.log

exec 2>$log

#---- function ------#-------------------------- reporting -----------------------#--------------------------#
test_cpedict_setup    ; ret_val=$? ; report_result "test_cpedict_setup"   $ret_val  ; result=$[$result+$ret_val]
test_cpedict_tc01     ; ret_val=$? ; report_result "test_cpedict_tc01"    $ret_val  ; result=$[$result+$ret_val]   
test_cpedict_tc02     ; ret_val=$? ; report_result "test_cpedict_tc02"    $ret_val  ; result=$[$result+$ret_val]   
test_cpedict_tc03     ; ret_val=$? ; report_result "test_cpedict_tc03"    $ret_val  ; result=$[$result+$ret_val]   
#test_cpedict_tc04     ; ret_val=$? ; report_result "test_cpedict_tc04"    $ret_val  ; result=$[$result+$ret_val]   
test_cpedict_tc05     ; ret_val=$? ; report_result "test_cpedict_tc04"    $ret_val  ; result=$[$result+$ret_val]   
test_cpedict_cleanup  ; ret_val=$? ; report_result "test_cpedict_cleanup" $ret_val  ; result=$[$result+$ret_val]

echo "------------------------------------------"
echo "See ${srcdir}/${log}"

exit $result
