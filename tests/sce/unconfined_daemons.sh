#!/usr/bin/env bash

# This script assumes that you have "ps", "grep" and "wc" available
# on your system.

# The regex simply goes over the columns making sure we are comparing
# the corrent one. This makes sure we avoid false positives with processes
# called initrc_t

if [ "`ps -eZ | grep "^[^:]*:[^:]*:initrc_t:" | wc -l`" == "0" ]; then
	exit $XCCDF_RESULT_PASS
else
	exit $XCCDF_RESULT_FAIL
fi

