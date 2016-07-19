#!/bin/bash

VA_RANDOMIZATION=1

if [ -a /proc/sys/kernel/randomize_va_space ] &&
   [ $VA_RANDOMIZATION -gt `cat /proc/sys/kernel/randomize_va_space` ]
then
    echo "Virtual addresses randomization is disabled."
    echo "You can use \"sysctl\" command to turn it on."

    exit $XCCDF_RESULT_FAIL
fi

exit $XCCDF_RESULT_PASS

