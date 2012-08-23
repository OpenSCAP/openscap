#!/bin/bash


. $srcdir/../../../test_common.sh

test_init test_api_xccdf_unittests.log
test_run "xccdf:complex-check -- NAND is working properly" ./test_xccdf_shall_pass $srcdir/test_xccdf_complex_check_nand.xccdf.xml
test_run "xccdf:complex-check -- single negation" ./test_xccdf_shall_pass $srcdir/test_xccdf_complex_check_single_negate.xccdf.xml
test_run "Certain id's of xccdf_items may overlap" ./test_xccdf_shall_pass $srcdir/test_xccdf_overlaping_IDs.xccdf.xml

test_run "Escaping of xml &amp within xccdf:value" $srcdir/test_xccdf_xml_escaping_value.sh
test_exit
