#!/usr/bin/env bash

# We do this dummy check to ensure $PATH is passed properly
#echo "abcde" | awk '/.+/ {print $1}' | grep "abcde"
which which
RET=$?

if [[ $RET != 0 ]] ; then
    exit $XCCDF_RESULT_FAIL
else
    exit $XCCDF_RESULT_PASS
fi
