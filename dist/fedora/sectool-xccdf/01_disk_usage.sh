#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
# disc_usage.sh                                               #
# ----------------------------------------------------------- #

# report warnning when percent usage greater than
[ "$PERCENT_LIMIT_WARNING" == "" ] && PERCENT_LIMIT_WARNING=80
# report error when percent usage greater than
[ "$PERCENT_LIMIT_ERROR"   == "" ] && PERCENT_LIMIT_ERROR=100

RET=$XCCDF_RESULT_PASS
while read  device blocks used available_capacity percent_used directory ; do
    percent_sign='%'
    percent_used_num_only=${percent_used%$percent_sign}

    if [ "$percent_used_num_only" -ge "$PERCENT_LIMIT_ERROR" ] ; then
	echo "Device $device mounted to $directory is full! There is $percent_used used!"
	echo "Please backup and delete unused files from this directory"
	RET=$XCCDF_RESULT_FAIL
    elif [ "$percent_used_num_only" -ge "$PERCENT_LIMIT_WARNING" ] ; then
	echo "Device $device mounted to $directory is going to be full! There is $percent_used used!"
	echo "Please backup and delete unused files from this directory"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
done <<EOF
`df -P | tail -n +2 | sort -n -k 5 -r`
EOF

exit $RET

