#!/bin/bash

# Copyright 2014--2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. ../test_common.sh

function test_illicit_function_use {
	codebase=$(find $top_srcdir/src/ -regex '.*\.[ch]x*')
	if grep xmlReaderForFile $codebase; then
		echo "xmlReaderForFile is not allowed within OpenSCAP project. Please make a use of oscap_source facility."
		return 1;
	fi
	if grep xmlTextReaderSetErrorHandler $codebase; then
		echo "xmlTextReaderSetErrorHandler is not allowed within OpenSCAP project. Please make a use of oscap_source facility."
		return 1;
	fi
	if grep xmlTextReaderReadString $codebase; then
		echo "xmlTextReaderReadString is not allowed within OpenSCAP project. Its implementation in libxml does not play well with xmlWalkerReader."
		return 1;
	fi
	whitelisted=$(echo $codebase | sed 's/\S*\/source\/oscap_source.c / /g' -)
	if grep xmlReadFile $whitelisted; then
		echo "xmlReadFile is not allowed within OpenSCAP project. Please make a use of oscap_source facility."
		return 1;
	fi
}

function shell_script_syntax(){
	for script in $@; do
		bash -n -u $top_srcdir/$script
	done
}

function test_config_h(){
	text='(#\s*include\s+<config.h>)|(#\s*include\s+"config.h")'

	ignore_list='(MurmurHash3.c|rbt_gen.c)$'
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

test_init "test_codebase.log"

test_run "illicit use of functions" test_illicit_function_use 0
test_run "Check syntax of distributed shell scripts" shell_script_syntax \
	utils/oscap-ssh utils/oscap-docker
test_run "Check existence including config.h in every .c file" test_config_h
test_exit
