#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
#                                                             #
# logfiles.sh                                                 #
# secTool script testing presence of some logfiles            #
# more info in logfiles.dsc                                   #
# ----------------------------------------------------------- #

# which log files to examine
# format: file_path owner group permissions
LOGFILES="/var/log/wtmp root utmp 664
	  /var/log/btmp root utmp 600
	  /var/log/lastlog root root 644
	  /var/run/utmp root utmp 664
	  /var/log/messages root root 600"

RET=$XCCDF_RESULT_PASS

while read file owner group perm
do
    # exists?
    if ! [[ -f $file ]]
    then
		echo "File $file doesn't exist!"
		echo "Please create this file."
		RET=$XCCDF_RESULT_FAIL
    else
		# owner
		if [[ "`stat -c '%U' $file`" != "$owner" ]]
		then
			echo "File $file has wrong owner. The correct owner is $owner."
			echo "Please change the owner of this file to the recommended one."
			RET=$XCCDF_RESULT_FAIL
		fi

		# group	
		if [[ "`stat -c '%G' $file`" != "$group" ]]
		then
			echo "File $file has wrong group. The correct group is $group."
			echo "Please change the group of this file to the recommended one."
			RET=$XCCDF_RESULT_FAIL
		fi

		# permission
		if [[ "`stat -c '%a' $file`" != "$perm" ]]
		then
			echo "File $file has wrong permssions! The correct permissions are $perm."
			echo "Please change the permissions of this file to the recommended value."
			RET=$XCCDF_RESULT_FAIL
		fi
	fi
done<<EOF
$LOGFILES
EOF

exit $RET

