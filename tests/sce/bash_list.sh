#!/usr/bin/env bash

ls -al /
RET=$?

if [[ $RET != 0 ]] ; then
    exit $XCCDF_RESULT_FAIL
else
    exit $XCCDF_RESULT_PASS
fi
