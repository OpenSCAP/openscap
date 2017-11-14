#!/bin/bash

# Copyright 2014--2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $builddir/tests/test_common.sh

function test_config_h(){
	text='(#\s*include\s+<config.h>)|(#\s*include\s+"config.h")'

	ignore_list='(MurmurHash3.c|rbt_gen.c|oval_testing_ext_probe.c)$'
	echo "Files from this mask will not be checked: $ignore_list"

	codebase=$(find $top_srcdir/src/ -name "*.c" | sort)
	echo "$codebase" | grep -vE "$ignore_list" | while read filename;
	do
		grep -E "$text" "$filename" --quiet || {
			echo "$filename does not contain '$text'"
			return 1
		}
	done
}

test_init "test_config_h.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "Check existence including config.h in every .c file" test_config_h
fi

test_exit
