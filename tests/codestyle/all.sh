#!/bin/bash

# Copyright 2014 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $srcdir/../test_common.sh

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
}

test_init "test_codebase.log"

test_run "illicit use of functions" test_illicit_function_use 0

test_exit
