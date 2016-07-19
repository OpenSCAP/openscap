#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# passwd.sh                                                   #
# more info in passwd.dsc                                     #
# ----------------------------------------------------------- #

# TODO:
#  are uids and gids in correct range? (find the range in file used in adduser from shadow utils..)
#  has filled fullname?
#  has valid shell? (/etc/shell)

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

# is it an ordinary user?
# takes one param - username
function canLogIn {
    if [[ "$(getent passwd "${1}" | cut -d: -f 7)" != "/sbin/nologin" ]]; then
	password="$(getent shadow "${1}" | cut -d: -f 2)"
	# length of passwd - very short means invalid password and disabled account
	if (( ${#password} < 13 )); then
	    return 1
	else
	    # 0 is used as true in this case
	    return 0
	fi
    else
	# 1 is used as false in this case
	return 1
    fi
}

RET=$XCCDF_RESULT_PASS

#constants
passwd=/etc/passwd
group=/etc/group
shadow=/etc/shadow
group_shadow=/etc/gshadow

# --- Error IDs ------- #

UID_MIN_VALUE="`egrep '^UID_MIN' /etc/login.defs | awk '{ print $2 }'`"
UID_MAX_VALUE="`egrep '^UID_MAX' /etc/login.defs | awk '{ print $2 }'`"
GID_MIN_VALUE="`egrep '^GID_MIN' /etc/login.defs | awk '{ print $2 }'`"
GID_MAX_VALUE="`egrep '^GID_MAX' /etc/login.defs | awk '{ print $2 }'`"

#if[[ `whoami` -ne 'root' ]]
if [[ $UID -ne '0' ]]
then
    echo "You have to be logged as root to run this test!"
    exit $XCCDF_RESULT_ERROR
fi

# permissions on /etc/passwd and /etc/group should be 644, should be owned by root:root
check_file_perm $passwd 644 root:root  1 $E_BAD_PERMISSIONS "User database"

i=0
while read line
do
	i=$[i+1]

	##### empty line #####
	if [[ "$line" == "" ]]
	then
		echo "$passwd: Line $i is empty"
		echo "Please delete this line."
		RET=$XCCDF_RESULT_FAIL
		continue
	fi

	##### number of fields #####
	nf=`echo "$line" | awk -F: '{printf NF}'`
	if [ "$nf" -ne "7" ]
	then
		echo "$passwd: Line $i has wrong number of fields"
		echo "Please see 'man 5 passwd' and correct this line."
		RET=$XCCDF_RESULT_FAIL
		continue
	fi	

	# now we can parse these fields, we know that all fields exist
	username="`echo $line | awk -F: '{print $1}'`"
	pass="`echo $line | awk -F: '{print $2}'`"
	uid="`echo $line | awk -F: '{print $3}'`"
	gid="`echo $line | awk -F: '{print $4}'`"
	fullname="`echo $line | awk -F: '{print $5}'`"
	homedir="`echo $line | awk -F: '{print $6}'`"
	shell="`echo $line | awk -F: '{print $7}'`"

	##### line has an empty login field #####
	if [[ "$username" == "" ]]
	then
		echo "$passwd: Line $i: missing username!"
		echo "Check this line, fill in first item (username), or delete the whole line."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### disallowed characters #####
	isValidName $username
	if [ $? -ne 1 ]
	then
		echo "$passwd: Line $i: User $username contains disallowed characters in his login."
		echo "Check this line and rename user's login to contain lowercase letters only."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### too long username #####
	getValueFromH '/usr/include/bits/utmp.h' 'UT_NAMESIZE'
	MaxLength=$?	
	length=$(echo "$username" | wc -m)
	if [ $length -gt $MaxLength ]
	then
		echo "$passwd: Line $i: User $username has too long username."
		echo "Check this line and rename user's login to be shorter than $MaxLength characters"
		RET=$XCCDF_RESULT_FAIL
	fi

	##### password empty #####
	if [[ "$pass" == "" ]]
	then
		echo "$passwd: Line $i: User $username has no password!"
		echo "Please use 'passwd' utility immediately to set his password!"
		RET=$XCCDF_RESULT_FAIL
	fi

	##### password not shadowed (x in field 2) #####
	if [[ "$pass" != "x" ]] && [[ "$pass" != "" ]] && [[ "$pass" != "*" ]] && [[ "$pass" != "!" ]]
	then
		echo "$passwd: Line $i: User ${username}'s password is not shadowed"
		echo "Please use pwconv utility for moving passwords from \$passwd to \$shadow"
		RET=$XCCDF_RESULT_FAIL
	fi


	##### is UID a number? #####
	if [[ "`echo $uid | grep -e '^-\?[0-9]\+$'`" != "$uid" ]]
	then
		echo "$passwd: Line $i: User ID of user $username is not a valid number"
		echo "Please correct the user ID in $passwd."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### is GID a number? #####
	if [[ "`echo $gid | grep -e '^-\?[0-9]\+$'`" != "$gid" ]]
	then
		echo "$passwd: Line $i: Group ID of user $username is not a valid number"
		echo "Please correct the group ID in $passwd."
		RET=$XCCDF_RESULT_FAIL
	else

		##### is UID and GID in the range? #####
		if canLogIn "$username" && [[ "$username" != "root" ]];	then
			if [ "$uid" -lt "$UID_MIN_VALUE" ]  || [ "$uid" -gt "$UID_MAX_VALUE" ]
			then
				echo "$passwd: Line $i: User $username has UID out of range"
				echo "Change UID of this user to be in the range <$UID_MIN_VALUE, $UID_MAX_VALUE> (ordinary users) or less than $UID_MIN_VALUE (daemons)."
				RET=$XCCDF_RESULT_FAIL
			fi

			if [ $gid -lt $GID_MIN_VALUE ]  || [ $gid -gt $GID_MAX_VALUE ]
			then
				echo "$passwd: Line $i: User $username has GID out of range"
				echo "Change GID of this user to be in the range <$GID_MIN_VALUE, $GID_MAX_VALUE> (ordinary users) or less than $GID_MIN_VALUE (daemons)."
				RET=$XCCDF_RESULT_FAIL
			fi
		fi
	fi

	##### someone has uid 0, but his login is not "root"
	if [[ "$uid" == "0" ]] && [[ "$username" != "root" ]]
	then
		echo "$passwd: Line $i: User $username has UID 0, but his login is not 'root'!"
		echo "If there are more users with UID 0 in $passwd (you should see them on next lines), delete all but the first. Then set his name to root."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### someone has uid 1, but his login is not "bin"
	if [[ "$uid" == "1" ]] && [[ "$username" != "bin" ]]
	then
		echo "$passwd: Line $i: User $username has UID 1, but his login is not 'bin'!"
		echo "If there are more users with UID 1 in $passwd (you should see them on next lines), delete all but the first. Then set his name to 'bin'."
		RET=$XCCDF_RESULT_FAIL
	fi


	##### someone has gid 0, but his login is not "root"
	if [[ "$gid" == "0" ]] && [[ "$username" != "root" ]] && [[ "$username" != "halt" ]] && [[ "$username" != "shutdown" ]] && [[ "$username" != "sync" ]] && [[ "$username" != "operator" ]]
	then
		echo "$passwd: Line $i: User $username has GID 0, but his login is not 'root'!"
		echo "If there are more users with GID 0 in $passwd (you should see them on next lines), delete all but the first. Then set his name to root."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### someone has gid 1, but his login is not "bin"
	if [[ "$gid" == "1" ]] && [[ "$username" != "bin" ]]
	then
		echo "$passwd: Line $i: User $username has GID 1, but his login is not 'bin'!"
		echo "If there are more users with GID 1 in $passwd (you should see them on next lines), delete all but the first. Then set his name to 'bin'."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### negative uid #####
	if [ $uid -lt 0 ]
	then
		echo "$passwd: Line $i: User $username has a negative user ID"
		echo "Set his UID to non-negative and not yet used value in the range <$UID_MIN_VALUE, $UID_MAX_VALUE> (ordinary users) or less than $UID_MIN_VALUE (daemons)."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### negative gid #####
	if [ $gid -lt 0 ]
	then
		echo "$passwd: Line $i: User $username has a negative group ID"
		echo "Set his GID to non-negative and not yet used value in the range <$GID_MIN_VALUE, $GID_MAX_VALUE> (ordinary users) or less than $GID_MIN_VAL (daemons)."
		RET=$XCCDF_RESULT_FAIL
	fi

	##### is shell OK? (is in /etc/shells) #####
	if [[ "$username"  != "halt" ]] && [[ "$username" != "sync" ]] && [[ "$username" != "shutdown" ]] && [[ "$username" != "news" ]]
	then
		shell_ok="no"

		    while read s
		    do
			if [[ "$shell" == "$s" ]]
			then
			    shell_ok="yes"
			fi
		    done<<EOF
`cat /etc/shells`
EOF

		if [[ "$shell_ok" != "yes" ]]
		then
			echo "$passwd: Line $i: User $username has strange shell $shell"
			echo "Set last field on this line to /bin/bash, or add the shell to /etc/shells"
			RET=$XCCDF_RESULT_FAIL
		fi
	fi
done<<EOF
`cat $passwd`
EOF

##### two users with the same UID #####
while read line 
do
	# ignore empty lines
	if [[ "`echo $line | egrep -c '^[0-9]+:$'`" == "1" ]]
	then
		continue
	fi
	line_nr=`echo $line | cut -d: -f1`

	# we will compare with previous line, because it is sorted
	# But when we read first (non-empty) line, we have to only save his UID 
	if [[ $first_nonempty != "false" ]]
	then
		prev_uid=`echo $line | cut -d: -f4`
		prev_username=`echo $line | cut -d: -f2`
		prev_line=`echo $line | cut -d: -f1`

		first_nonempty="false"
		continue
	fi

	uid=`echo $line | cut -d: -f4`
	user=`echo $line |cut -d: -f2`
	line=`echo $line |cut -d: -f1`

	if [[ $uid == $prev_uid ]]
	then
		echo "User $user (passwd line $line) has the same UID ($uid) as user $prev_username (passwd line $prev_line)"
		echo "Please change UID of this user and don't forget to chown his home directory"
		RET=$XCCDF_RESULT_FAIL
	fi
	prev_uid=$uid
	prev_line=$line
	prev_username=$user
done<<EOF
`cat $passwd | awk '{ printf "%u:%s\n",NR,$0  }' | sort -n -t: -k4`
EOF
# ^^^
# we want to know line numbers of coliding records, so we can't use uniq -d...
# this also works with empty lines in passwd

##### two users with same username #####
while read user
do
	if [[ "$user" != "" ]]
	then
		lines="`grep -n -e "^$user:" $passwd | awk -F: '{ print $1 }'| tr '\n' ','`"
		lines="${lines%','}" # delete last coma
		echo "Duplicate login '$user' ($passwd lines $lines)"
		echo "Please change usernames on these lines to be different or delete duplicate records"
		RET=$XCCDF_RESULT_FAIL
	fi
done<<EOF
`awk -F: '{ if ($1 != "") print $1 }' $passwd | sort | uniq -d`
EOF

##### has root UID 0? #####
root_uid=`grep '^root:' $passwd |cut -d: -f3` 
if [[ $root_uid != "0" ]]
then
	echo "User root has UID $root_uid, but should have 0"
	echo "Change root's UID to 0"
	RET=$XCCDF_RESULT_FAIL
fi

exit $RET

