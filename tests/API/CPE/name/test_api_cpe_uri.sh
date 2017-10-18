#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap CPE URI Module Test Suite.
#
# Authors:
#      Ondrej Moris <omoris@redhat.com>

. $builddir/tests/test_common.sh

CPE_URIS=(`grep "cpe:" $srcdir/dict.xml | \
    sed 's/^.*cpe:/cpe:/g' | sed 's/".*$//g' | tr '\n' ' '`)

# Test cases.

function test_api_cpe_uri_smoke {
    ./test_api_cpe_uri --smoke-test
}

function test_api_cpe_uri_parse {
    require "grep" || return 255
    local ret_val=0
    for URI in ${CPE_URIS[@]}; do
	./test_api_cpe_uri --parsing  $URI parsing.out >/dev/null
	if [ $? -eq 0 ]; then
	    if [ ! "`cat  parsing.out`X" == "${URI}X" ]; then
		echo "`cat  parsing.out`X != ${URI}X"
		ret_val=1
	    fi
	else
	    echo "Parsing failed!"
	    ret_val=1
	fi	
    done
    return $ret_val
}

function test_api_cpe_uri_create {
    require "grep" || return 255
    local ret_val=0
    for URI in ${CPE_URIS[@]}; do
	./test_api_cpe_uri --parsing  $URI parsing.out > parsing.out.1
	if [ $? -eq 0 ]; then
	    if [ "`cat  parsing.out`X" == "${URI}X" ]; then
		sed 's/^\s*//g' parsing.out.1 > parsing.out.sed-tmp ; mv parsing.out.sed-tmp parsing.out.1
		sed 's/(null)//g' parsing.out.1 > parsing.out.sed-tmp ; mv parsing.out.sed-tmp parsing.out.1
		cat parsing.out.1 | xargs -d '\n' ./test_api_cpe_uri --creation > creation.out
 		if [ ! $? -eq 0 ] || [ "${URI}X" != "`cat creation.out`X" ]; then
		    echo "${URI}X != `cat creation.out`X"
		    ret_val=1; 
		fi
	    else
		echo "`cat  parsing.out`X != ${URI}X"
		ret_val=1
	    fi
	else
	    echo "Parsing failed!"
	    ret_val=1
	fi
    done
    return $ret_val
}

function test_api_cpe_uri_match {
    require "grep" || return 255
    local ret_val=0
    for URI in ${CPE_URIS[@]}; do
	./test_api_cpe_uri --parsing $URI parsing.out
	if [ $? -eq 0 ]; then
	    if [ "`cat parsing.out`X" == "${URI}X" ]; then
		CPE=(`cat parsing.out | sed 's/(null)//g' | \
		    sed 's/^\s*//g' | tr '\n' ' '`)
		./test_api_cpe_uri --matching $URI " " >matching.out
		if [ ! $? -eq 0 ] || ! grep "Mismatch" matching.out > /dev/null; then
		    echo "Cannot match URI \"$URI\"!"
		    ret_val=1;		
		fi
		if [ ! $? -eq 0 ] || ! grep "Mismatch" matching.out > /dev/null; then
		./test_api_cpe_uri --matching $URI "cpe:/:::::::" > matching.out
		    echo "Cannot match URI \"$URI\"!"
		    ret_val=1;		
		fi
		if [ ! $? -eq 0 ] || ! grep "Mismatch" matching.out > /dev/null; then
		./test_api_cpe_uri --matching $URI "cpe:/:foo" > matching.out
		    echo "Cannot match URI \"$URI\"!"
		    ret_val=1;		
		fi
		if [ ! $? -eq 0 ] || ! grep "Mismatch" matching.out > /dev/null; then
		./test_api_cpe_uri --matching $URI "cpe:/foo" > matching.out
		    echo "Cannot match URI \"$URI\"!"
		    ret_val=1;		
		fi

		OTHER_URIS=(`echo ${CPE_URIS[@]} | tr ' ' '\n' | grep -v $URI | \
		    tr '\n' ' '`)

		if [ ! $? -eq 0 ] || ! grep "Mismatch" matching.out > /dev/null; then
		./test_api_cpe_uri --matching $URI ${OTHER_URIS[@]} > matching.out
		    echo "Cannot match URI \"$URI\"!"
		    ret_val=1;		
		fi
		if [ ! $? -eq 0 ] || ! grep "Mismatch" matching.out > /dev/null; then
		./test_api_cpe_uri --matching $URI $URI > matching.out
		    echo "Cannot match URI \"$URI\"!"
		    ret_val=1;		
		fi
		if [ ! $? -eq 0 ] || ! grep "Mismatch" matching.out > /dev/null; then
		./test_api_cpe_uri --matching $URI ${CPE_URIS[@]} > matching.out
		    echo "Cannot match URI \"$URI\"!"
		    ret_val=1;		
		fi
	    else
		ret_val=1
		echo "Parsing failed!"
	    fi
	else
	    ret_val=1
	    echo "Parsing failed!"
	fi
    done
    return $ret_val
}

# Testing.

test_init "test_api_cpe_uri.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_api_cpe_uri_smoke"  test_api_cpe_uri_smoke
    test_run "test_api_cpe_uri_parse"  test_api_cpe_uri_parse
    test_run "test_api_cpe_uri_create" test_api_cpe_uri_create
    test_run "test_api_cpe_uri_match"  test_api_cpe_uri_match
fi

test_exit
