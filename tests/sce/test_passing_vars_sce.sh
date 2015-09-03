#!/bin/sh
#
# This is an SCE script used to test whether the value of a variable is passed
# to this script corretly or not. It just prints the value.

echo VALUE_1=$XCCDF_VALUE_VALUE_1
echo VALUE_2=$XCCDF_VALUE_VALUE_2
exit $XCCDF_RESULT_PASS
