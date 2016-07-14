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

. ../../test_common.sh

# Test Cases.

function test_probes_file {

    probecheck "file" || return 255

    local ret_val=0;
    local DF="$srcdir/test_probes_file.xml"
    local RF="results.xml"

    [ -f $RF ] && rm -f $RF

	if [ "$1" == "verbose" ];
	then
		VF="verbose"
		$OSCAP oval eval --verbose DEVEL --verbose-log-file $VF --results $RF $DF
	else
		$OSCAP oval eval --results $RF $DF
	fi

    if [ -f $RF ]; then
	verify_results "def" $DF $RF 13 && verify_results "tst" $DF $RF 204
	ret_val=$?
    else
	ret_val=1
    fi

    return $ret_val
}

function test_probes_file_filenames {

	probecheck "file" || return 255

	local ret_val=0
	local DF="$srcdir/test_probes_file_filename.xml"
	result="results.xml"
	files_dir=$(mktemp -d)
	DF_INJECTED=$(mktemp)

	echo "Files dir:	${files_dir}"
	echo "Content file:	${DF_INJECTED}"

	# create ascii files - should be processed correctly
	pushd "$files_dir" && {
		for i in {1..127};
		do
			[ $i == 47 ] && continue ; # '/' cannot be in filename
			hex=\\x$(printf "%x" "$i")
			touch "$(printf filename_ok_${i}_$hex$hex$hex)"
		done
		popd
	}

	# inject real path to content
	sed "s;<!--injected-path -->;${files_dir};" "$DF" > $DF_INJECTED

	$OSCAP oval eval --results $result $DF_INJECTED || ret_val=1
	$OSCAP oval validate $result || ret_val=1

	# some of files maybe weren't successfully created
	# (possibly not supported filenames by filesystem)
	# get real count of them
	filesCount=$(find "$files_dir" -type f | wc -l)

	assert_exists 1 '//results//criterion' || ret_val=1
	assert_exists 1 '//results//criterion[@result="true"]' || ret_val=1
	assert_exists "$filesCount" '//unix-sys:file_item' || ret_val=1

	rm $DF_INJECTED
	rm -rf "$files_dir"

	return $ret_val
}

function test_probes_file_invalid_utf8 {

	probecheck "file" || return 255

	local ret_val=0
	local DF="$srcdir/test_probes_file_filename.xml"
	result="results.xml"
	files_dir=$(mktemp -d)
	DF_INJECTED=$(mktemp)

	echo "Files dir:	${files_dir}"
	echo "Content file:	${DF_INJECTED}"

	# known invalid utf-8 sequence
	touch $(printf "${files_dir}/filename_\xf0\x28\x8c\xbc") || ret_val=1

	# inject real path to content
	sed "s;<!--injected-path -->;${files_dir};" "$DF" > $DF_INJECTED

	$OSCAP oval eval --results $result $DF_INJECTED || ret_val=1
	$OSCAP oval validate $result || ret_val=1

	assert_exists 1 '//results//criterion' || ret_val=1
	assert_exists 1 '//results//criterion[@result="error"]' || ret_val=1

	rm $DF_INJECTED
	rm -rf "$files_dir"

	return $ret_val
}

# Testing.

test_init "test_probes_file.log"

test_run "test_probes_file with verbose mode" test_probes_file verbose
test_run "test_probes_file" test_probes_file
test_run "test_probes_file_filenames" test_probes_file_filenames
test_run "test_probes_file_invalid_utf8" test_probes_file_invalid_utf8

test_exit
