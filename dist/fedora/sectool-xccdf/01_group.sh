#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
#                                                             #
# group.sh                                                    #
# more info in group.dsc                                      #
# ----------------------------------------------------------- #

#constants
passwd=/etc/passwd
group=/etc/group
shadow=/etc/shadow
group_shadow=/etc/gshadow

if [[ $UID -ne '0' ]]
then
    echo "You have to be logged as root to run this test!"
    exit ${XCCDF_RESULT_ERROR}
fi

RET=$XCCDF_RESULT_PASS

# gets a value of a constant defined in a c/c++ header file by #define
# usage example: 
# getValueFromH '/usr/include/bits/utmp.h' 'UT_NAMESIZE'
# echo $ReturnVal
function getValueFromH {
	if ! [[ -r "$1" ]]; then
		report 'WARNING' 1234 "Can't read a constant $2, header file $1 not found"
		return 0
	else
		line="$(egrep "^#define $2..*" $1)"
		if [[ -n "$line" ]]; then
			local -i retval=$(echo "$line" | cut -f2)			
			return $retval
		else
			report 'WARNING' 1234 "Can't read a constant $2 from file $1, definition of the constant not found in this file"
			return 0
		fi
	fi
}

# function isValidName
# tests a string whether it is a valid group/user name
# 1 - true
# 0 - false
function isValidName {
	# first we need to set LC_ALL to C to get ranges working case-sensitively
	oldLC_ALL=${LC_ALL}
	LC_ALL="C"

	# this constant contains a regex which recognizes, if the string is valid name of user or group
	allowedNamesRegex='^[a-zA-Z0-9_.][a-zA-Z0-9_.-]*[a-zA-Z0-9_.$-]?$'
	echo $1 | egrep -q "$allowedNamesRegex"

	returnValue=$[ 1 - $? ]
	LC_ALL=${oldLC_ALL}
	return $returnValue
}

UID_MIN_VALUE="`egrep '^UID_MIN' /etc/login.defs | awk '{ print $2 }'`"
UID_MAX_VALUE="`egrep '^UID_MAX' /etc/login.defs | awk '{ print $2 }'`"
GID_MIN_VALUE="`egrep '^GID_MIN' /etc/login.defs | awk '{ print $2 }'`"
GID_MAX_VALUE="`egrep '^GID_MAX' /etc/login.defs | awk '{ print $2 }'`"

i=0
while read line
do
	i=$[i+1]

	##### empty line #####
	if [[ "$line" == "" ]]
	then
		echo "$shadow: Line $i is empty"
		echo "Please delete this line."
		[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
		continue
	fi

	##### number of fields #####
	nf=`echo "$line" | awk -F: '{printf NF}'`
	if [ "$nf" -ne "4" ]
	then
		echo $line
		echo "$shadow: Line $i has wrong number of fields"
		echo "Please see 'man group' and correct this line."
		RET=$XCCDF_RESULT_FAIL
		continue
	fi

	# now we can parse these fields, we know that all fields exist
	groupname="`echo $line | awk -F: '{print $1}'`"
	pass="`echo $line | awk -F: '{print $2}'`"
	gid="`echo $line | awk -F: '{print $3}'`"
	users="`echo $line | awk -F: '{print $4}'`"

	##### line has an empty groupname field #####
	if [[ "$groupname" == "" ]]
	then
		echo "$group: Line $i: missing groupname!"
		echo "Check this line, fill in first item (groupname), or delete whole line."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### disallowed characters #####
	isValidName $groupname
	if [ $? -ne 1 ]
	then
		echo "$shadow: Line $i: Group name $groupname contains disallowed characters."
		echo "Check this line and rename group name to contain lowercase letters only. Don't forget to update the $group_shadow file!"
		RET=$XCCDF_RESULT_FAIL
	fi

	##### too long groupname #####
	getValueFromH '/usr/include/bits/utmp.h' 'UT_NAMESIZE'
	MaxLength=$?
	length=`echo "$groupname" | wc -m`
	if [ $length -gt $MaxLength ]
	then
		echo "$group: Line $i: User $groupname has too long group name."
		echo "Check this line in $group and rename group name to be shorter than $MaxLength characters"
		RET=$XCCDF_RESULT_FAIL
	fi

	##### password not shadowed #####
	if [[ "$pass" != "x" ]] && [[ "$pass" != "" ]]
	then
		echo "$group: Line $i: Group $groupname has a password but it is not shadowed!"
		echo "Please use grpconv utility for moving group passwords from $group to $group_shadow"
		RET=$XCCDF_RESULT_FAIL
	fi

	##### is GID a number? #####
	if [[ "`echo $gid | grep -e '^-\?[0-9]\+$'`" != "$gid" ]]
	then
		echo "$group: Line $i: Group ID of the group $groupname is not a valid number"
		echo "Please correct the group ID."

		RET=$XCCDF_RESULT_FAIL
	else

		##### is UID and GID in the range? #####
		if [[ "$groupname" != "nfsnobody" ]] 
		then
			if (( $gid < 0 || $gid > $GID_MAX_VALUE )); then
				echo "$group: Line $i: Group $groupname has GID out of range"
				echo "Change GID of this group to be in the range <0, $GID_MAX_VALUE>"

				RET=$XCCDF_RESULT_FAIL
			fi

		fi
	fi

done<<EOF
`cat $group`
EOF

##### two groups with the same group name #####
while read groupname
do
	if [[ "$groupname" != "" ]]
	then
		lines="`grep -n -e "^$groupname:" $group | awk -F: '{ print $1 }'| tr '\n' ','`"
		lines="${lines%','}" # delete last coma
		echo "Duplicate group name '$groupname' (lines $lines)"
		echo "Please change groupnames on these lines to be different or delete duplicate records"
		RET=$XCCDF_RESULT_FAIL
	fi
done<<EOF
`awk -F: '{ if ($1 != "") print $1 }' $group | sort | uniq -d`
EOF

##### two groups with the same GID #####
while read groupID
do
	if [[ "$groupID" != "" ]]
	then
		lines="`grep -n -e "^.*:.*:$groupID:.*$" $group | awk -F: '{ print $1 }'| tr '\n' ','`"
		lines="${lines%','}" # delete last coma
		echo "Duplicate group IDs '$groupID' (lines $lines)"
		echo "Please change IDs on these lines to be different or delete duplicate records"
		RET=$XCCDF_RESULT_FAIL
	fi
done<<EOF
`awk -F: '{ if ($3 != "") print $3 }' $group | sort | uniq -d`
EOF

##### has root GID 0? #####
root_gid=`grep '^root:' $group |cut -d: -f3` 
if [[ $root_gid != "0" ]]
then
	echo "User root has GID $root_gid, but should have 0"
	echo "Change root's GID to 0"
	RET=$XCCDF_RESULT_FAIL
fi

exit $RET

