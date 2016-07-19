#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
#                                                             #
# passwd.sh                                                   #
# more info in passwd.dsc                                     #
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

function check_file_perm () {
    if [[ -a "${1}" ]]; then
	local -i CPERM=$(stat -c '%a' "${1}")

	if (( ${CPERM} != $2 )); then
	    if (( (8#${CPERM} | 8#${2}) == 8#${2} )); then 	
		if (( ${4} == 1 )); then
		    echo "Permissions on $(stat -c '%F' "${1}") \"${1}\" are more restrictive than required: ${CPERM} (${6:-uknown}, required persmissions are ${2})"
		fi
	    else
		if (( ${4} == 1 )); then
		    echo "Wrong permissions on $(stat -c '%F' "${1}") \"${1}\": ${CPERM} (${6:-unknown}, required permissions are ${2})"
		    RET=$XCCDF_RESULT_FAIL
		fi
	    fi
	fi

	if ! (stat -c '%U:%G' "${1}" | grep -q "${3}"); then
	    if (( ${4} == 1 )); then
		echo "Wrong owner/group on $(stat -c '%F' "${1}"): \"${1}\" (${6:-unknown}, required owner/group is ${3})"
		RET=$XCCDF_RESULT_FAIL
	    fi
	fi
    else
	if (( ${4} == 1 )); then
	    echo "Missing file or directory: \"${1}\" (${6:-unknown})"
	    RET=$XCCDF_RESULT_FAIL
	fi
    fi
}

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

# permissions on /etc/shadow and /etc/gshadow should be 400, should be owned by root:root
check_file_perm $shadow 000 root:root  1 $E_BAD_PERMISSIONS_SHADOW "User shadow database"
check_file_perm $group_shadow 000 root:root  1 $E_BAD_PERMISSIONS_SHADOW "Group shadow database"

i=0
while read line
do
	i=$[i+1]

	##### empty line #####
	if [[ "$line" == "" ]]
	then
		echo "$shadow: Line $i is empty"
		echo "Please delete this line."
		[[ "$RET" == $XCCDF_RESULT_FAIL ]] | RET=$XCCDF_RESULT_INFORMATIONAL
		continue
	fi

	##### number of fields #####
	nf=`echo "$line" | awk -F: '{printf NF}'`
	if [ "$nf" -ne "9" ]
	then
		echo $line
		echo "$shadow: Line $i has wrong number of fields"
		echo "Please see 'man 5 shadow' and correct this line."
		RET=$XCCDF_RESULT_FAIL
		continue
	fi	

	# now we can parse these fields, we know that all fields exist
	username="`echo $line | awk -F: '{print $1}'`"
	pass="`echo $line | awk -F: '{print $2}'`"
	uid="`echo $line | awk -F: '{print $3}'`"
	gid="`echo $line | awk -F: '{print $4}'`"

	##### line has an empty login field #####
	if [[ "$username" == "" ]]
	then
		echo "$shadow: Line $i: missing username!"
		echo "Check this line, fill in first item (username), or delete whole line."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### disallowed characters #####
	isValidName $username
	if [ $? -ne 1 ]
	then
		echo "$shadow: Line $i: User $username contains disallowed characters in his login."
		echo "Check this line and rename user's login to contain lowercase letters only."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### too long username #####
	getValueFromH '/usr/include/bits/utmp.h' 'UT_NAMESIZE'
	MaxLength=$?
	length=`echo "$username" | wc -m`
	if [ $length -gt $MaxLength ]
	then
		echo "$shadow: Line $i: User $username has too long username."
		echo "Check this line in $shadow and rename user's login to be shorter than $MaxLength characters"
		RET=$XCCDF_RESULT_FAIL
	fi

	##### password empty #####
	if [[ "$pass" == "" ]]
	then
		echo "$shadow: Line $i: User $username has no password!"
		echo "Run 'passwd' utility immediately to set his password!"
		RET=$XCCDF_RESULT_FAIL
	fi
done<<EOF
`cat $shadow`
EOF

##### two users with same username #####
while read user
do
	if [[ "$user" -ne "" ]]
	then
		lines="`grep -n -e "^$user:" $shadow | awk -F: '{ print $1 }'| tr '\n' ','`"
		lines="${lines%','}" # remove last comma
		echo "Duplicate login '$user' ($shadow lines $lines)"
		echo "Please change usernames on these lines to be different or delete duplicate records"
		RET=$XCCDF_RESULT_FAIL
	fi
done<<EOF
`awk -F: '{ if ($1 != "") print $1 }' $shadow | sort | uniq -d`
EOF

exit $RET

