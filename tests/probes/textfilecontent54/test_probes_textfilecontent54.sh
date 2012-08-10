#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.
#
# Created on: Nov 30, 2009
#
# Authors:
#      Peter Vrabec, <pvrabec@redhat.com>
#      David Niemoller
#      Ondrej Moris, <omoris@redhat.com>

. ${srcdir}/../../test_common.sh

# Test Cases.

function test_probes_textfilecontent54 {

    probecheck "textfilecontent54" || return 255
	
    local ret_val=0;
    local DF="${srcdir}/test_probes_textfilecontent54.xml"
    local RF="results.xml"
    
    [ -f $RF ] && rm -f $RF

    local FILE_A="/tmp/test_probes_textfilecontent54.tmp_file"
    local FILE_B="/tmp/test_probes_textfilecontent54.tmp_file_empty"
    local FILE_C="/tmp/test_probes_textfilecontent54.tmp_file_non_existing"

    touch "$FILE_A"
    touch "$FILE_B"

    echo "valid_key = valid_value" > "$FILE_A"
    echo "valid_key = valid_value" >> "$FILE_A"

    ../../../utils/.libs/oscap oval eval --results $RF $DF
    
    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 16
	ret_val=$?
    else 
	ret_val=1
    fi

    return $ret_val
}

LST_VALID="
	tfc54-def-5.4-valid.xml
	tfc54-def-5.5-valid.xml
	tfc54-def-5.6-valid.xml
	tfc54-def-5.7-valid.xml
	tfc54-def-5.8-valid.xml
	tfc54-def-5.9-valid.xml
	tfc54-def-5.10-valid.xml
	tfc54-def-5.10.1-valid.xml
"

LST_INVALID="
	tfc54-def-5.4-invalid.xml
"

function tfc54_validation {
	local ret=0

	probecheck "textfilecontent54" || return 255

	echo "*** Validating and evaluating correct OVAL content:"
	for i in $LST_VALID; do
		echo "Validating definitions '$i'."
		../../../utils/.libs/oscap oval validate-xml "$i" \
			|| { echo "Validation failed."; ret=1; continue; }
		echo "Evaluating definitions '$i'."
		r="$(basename $i .xml)-results.xml"
		../../../utils/.libs/oscap oval eval --results "$r" "$i" \
			|| { echo "Evaluation failed."; ret=1; continue; }
		echo "Validating results '$r'."
		../../../utils/.libs/oscap oval validate-xml --results "$r" \
			|| { echo "Validation failed."; ret=1; }
	done

	echo "*** Validating incorrect OVAL content:"
	for i in $LST_INVALID; do
		echo "Validating content '$i'."
		../../../utils/.libs/oscap oval validate-xml "$i" \
			&& { echo "Validation incorrectly succeeded."; ret=1; }
	done

	return $ret
}

# Testing.

test_init "test_probes_textfilecontent54.log"

test_run "test_probes_textfilencontent54" test_probes_textfilecontent54
export OSCAP_FULL_VALIDATION=1
test_run "textfilecontent54 validation" tfc54_validation

test_exit
