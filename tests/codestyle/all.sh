#!/usr/bin/env bash

# Copyright 2014--2015 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $builddir/tests/test_common.sh

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

	codebase_without_xml_read_file=$(echo $codebase | xsed 's/\S*\/source\/oscap_source.c / /g' -)
	if grep xmlReadFile $codebase_without_xml_read_file; then
		echo "xmlReadFile is not allowed within OpenSCAP project. Please make a use of oscap_source facility."
		return 1;
	fi
}

function shell_script_syntax(){
	for script in $@; do
		bash -n -u $top_srcdir/$script
	done
}

test_init "test_codebase.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "illicit use of functions" test_illicit_function_use 0
    test_run "Check syntax of distributed shell scripts" shell_script_syntax \
	    utils/oscap-ssh
fi

test_exit
