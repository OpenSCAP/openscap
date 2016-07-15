#!/bin/bash

SELINUX_STATE=`/usr/sbin/getenforce | tr '[A-Z]' '[a-z]'`

if [[ -z "$XCCDF_VALUE_EXPECTED_STATE" ]]; then
    XCCDF_VALUE_EXPECTED_STATE="enforcing"
    echo "WARNING: Using default expected state!"
else
    XCCDF_VALUE_EXPECTED_STATE="$(echo $XCCDF_VALUE_EXPECTED_STATE | tr '[A-Z]' '[a-z]')"
fi

if [[ $SELINUX_STATE != $XCCDF_VALUE_EXPECTED_STATE ]]
then
	echo "Selinux is in "$SELINUX_STATE" state."
	echo "Expected state: "$XCCDF_VALUE_EXPECTED_STATE

	if [[ $XCCDF_VALUE_EXPECTED_STATE == "enforcing" ]]; then
	    echo "Using Enforing state is highly recommended. See selinux manual page for switching to Enforcing state."
	fi

	exit $XCCDF_RESULT_FAIL
fi

exit $XCCDF_RESULT_PASS
