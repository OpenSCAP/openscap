#!/usr/bin/env bash

SELINUX_MODE=`/usr/sbin/getenforce`

if [[ $SELINUX_MODE != "Enforcing" ]]
then
	echo "Selinux is in "$SELINUX_MODE" mode."
	echo "Using Enforing mode is highly recommended. See selinux manual page for switching to Enforcing mode."

	exit $XCCDF_RESULT_FAIL
fi

exit $XCCDF_RESULT_PASS

