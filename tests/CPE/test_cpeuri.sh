#!/bin/sh

# Copyright 2008 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CPE URI Module Test Suite.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>
#
# TODO: 
#      set function in more detail

. ${srcdir}/test_common.sh

# Setup.
function test_cpeuri_setup {
    local ret_val=0
    
    CPE_URIS=(`grep "cpe:" ${srcdir}/CPE/dict.xml | sed 's/^.*cpe:/cpe:/g' | sed 's/".*$//g' | tr '\n' ' '`)

    return $ret_val
}

# Test Cases.

function test_cpeuri_tc01 {
    local ret_val=0

    ./test_cpeuri --sanity-check 
    ret_val=$?

    return $ret_val
}

function test_cpeuri_tc02 {
    local ret_val=0

    for URI in ${CPE_URIS[@]}; do
	./test_cpeuri --parsing  $URI parsing.out.0 > parsing.out.1 
	if [ $? -eq 0 ]; then
	    if [ `cat  parsing.out.0` = "$URI" ]; then
		sed 's/(null)//g' parsing.out.1 > parsing.out
		CPE=(`cat parsing.out | sed 's/^\s*//g' | tr '\n' ' '`)
		
		./test_cpeuri --creation  "${CPE[0]}" "${CPE[1]}" "${CPE[2]}" "${CPE[3]}" "${CPE[4]}" "${CPE[5]}" "${CPE[6]}" > creation.out
 		if [ ! $? -eq 0 ] || [ ! ${URI} = `cat creation.out` ]; then ret_val=1; fi
		
		./test_cpeuri --matching $URI " " > matching.out
		if [ ! $? -eq 0 ] || ! grep -q "Mismatch" matching.out; then ret_val=1; fi
		
		./test_cpeuri --matching $URI "cpe:/:::::::" > matching.out
		if [ ! $? -eq 0 ] || ! grep -q "Mismatch" matching.out; then ret_val=1; fi
		
		./test_cpeuri --matching $URI "cpe:/:foo" > matching.out
		if [ ! $? -eq 0 ] || ! grep -q "Mismatch" matching.out; then ret_val=1; fi
		
		./test_cpeuri --matching $URI "cpe:/foo" > matching.out
		if [ ! $? -eq 0 ] || ! grep -q "Mismatch" matching.out; then ret_val=1; fi
		
		OTHER_URIS=(`echo ${CPE_URIS[@]} | tr ' ' '\n' | grep -v $URI | tr '\n' ' '`)
		./test_cpeuri --matching $URI ${OTHER_URIS[@]} > matching.out
		if [ ! $? -eq 0 ] || ! grep -q "Mismatch" matching.out; then ret_val=1; fi
		
		./test_cpeuri --matching $URI $URI > matching.out
		if [ ! $? -eq 0 ] || ! grep -q "Match" matching.out; then ret_val=1; fi
		
		./test_cpeuri --matching $URI ${CPE_URIS[@]} > matching.out
		if [ ! $? -eq 0 ] || ! grep -q "Match" matching.out; then ret_val=1; fi
	    else
		ret_val=1
	    fi
	else
	    ret_val=1
	fi
    done
    return $ret_val
}

# Cleanup.
function test_cpeuri_cleanup {
    local ret_val=0

    rm creation.out parsing.out parsing.out.0 parsing.out.1 matching.out

    return $ret_val
}

# TESTING.

echo "------------------------------------------"

result=0
log=test_cpeuri.log

exec 2>$log

#---- function ------#-------------------------- reporting -----------------------#--------------------------#
test_cpeuri_setup    ; ret_val=$? ; report_result "test_cpeuri_setup"   $ret_val  ; result=$[$result+$ret_val]
test_cpeuri_tc01     ; ret_val=$? ; report_result "test_cpeuri_tc01"    $ret_val  ; result=$[$result+$ret_val]   
test_cpeuri_tc02     ; ret_val=$? ; report_result "test_cpeuri_tc02"    $ret_val  ; result=$[$result+$ret_val]   
test_cpeuri_cleanup  ; ret_val=$? ; report_result "test_cpeuri_cleanup" $ret_val  ; result=$[$result+$ret_val]

echo "------------------------------------------"
echo "See ${srcdir}/${log}"

exit $result
