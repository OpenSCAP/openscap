#!/bin/sh

. $srcdir/../../../test_common.sh

test_init test_api_xccdf_unittests.log
test_run "xccdf:complex-check -- NAND is working properly" ./test_xccdf_shall_pass test_xccdf_complex_check_nand.xccdf.xml
test_exit
