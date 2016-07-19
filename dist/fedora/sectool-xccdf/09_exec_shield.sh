#!/bin/bash

EXEC_SHIELD=1

if [ -a /proc/sys/kernel/exec-shield ] && 
   [ $EXEC_SHIELD -ne `cat /proc/sys/kernel/exec-shield` ]
then
    echo "Exec-shield is disabled."
    echo "You can use \"sysctl\" command to turn it on."

    exit $XCCDF_RESULT_FAIL
fi

exit $XCCDF_RESULT_PASS

