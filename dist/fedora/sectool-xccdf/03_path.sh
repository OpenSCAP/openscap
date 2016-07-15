#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
#                                                             #
# path.sh                                                     #
# secTool script for testing directories in $PATH             #
# more info in path.dsc                                       #
# ----------------------------------------------------------- #

RET=$XCCDF_RESULT_PASS

#
# a little workaround to get real PATH, not the one got by userhelper
# 

REALPATH="`bash -cl 'echo "$PATH"' | tail -1`"

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

# Test for dangerous path
# test_dangerous_path path_str path_re hint_msg
function test_dangerous_path {
	local path_str="$1"
	local re="$2"
	local hint_msg="$3"
	local msg=""

	if echo "$REALPATH" | egrep "(:|^)($re)(:|$)" >/dev/null
	then
		echo "$path_str found in variable PATH!"
		printf -v msg "$hint_msg" "$path_str"
		echo "$msg"
		RET=$XCCDF_RESULT_FAIL
	fi
}

test_dangerous_path 'Empty element'  ''            "%s allows running binaries from the current directory without specifying full path"
test_dangerous_path 'Relative path'  '[^/~:][^:]*' "%s allows running binaries depending on current working directory"
test_dangerous_path 'Directory /tmp' '/tmp/?'      "%s shouldn't be specified in path as it is usually world-writable"

# test of presence and write permission of all dirs in PATH
while read -d: dir
do
    # does the directory exist?
    if ! [[ -d $dir ]]
    then
	echo "Path variable contains directory $dir, which doesn't exist or is not an directory."
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    else
	    perm="`stat -c '%A' $dir`"
	    if [[ "${perm:5:1}" == "w" ]]
	    then
		echo "Path variable contains directory $dir, which is group-writable"
		echo "Please look in your startup scripts and remove this setting. This directory can be used to make you execute a fake (and dangerous) binary file, i.e. ls"
		RET=$XCCDF_RESULT_FAIL
	    fi

	    if [[ "${perm:8:1}" == "w" ]]
	    then
		echo "Path variable contains directory $dir, which is world-writable"
		echo "Please look in your startup scripts and remove this setting. This directory can be used to make you execute a fake (and dangerous) binary file, i.e. ls"
		RET=$XCCDF_RESULT_FAIL
	    fi

	    # group or others writable executable
	    while read file
	    do
		    if [[ -f "$file" ]]
		    then
			    echo "File ${file}, which is placed in PATH directory ${dir}, is writable NOT only by his owner! The correct permissions should be 755."
			    echo "This is VERY dangerous, since one can change contents of this executable. Please set permissions of this file to the recommended one immediately!"
			    RET=$XCCDF_RESULT_FAIL
		    fi
	    done<<EOF
`find -L $dir -perm /o+w -perm /g+w`
EOF

	  # owner other than root and can log in
	  while read file
	  do
		if [[ "$file" -ne "" ]]
		then
			user="`stat -c '%U' $file`"
			if canLogIn "$user"; then
				echo "File ${file}, which is placed in PATH directory ${dir}, is owned by someone else than by root!"
				echo "This is VERY dangerous, since one can change contents of this executable. Please change the owner and group of this file to 'root' immediately!"
				RET=$XCCDF_RESULT_FAIL
			fi
		fi
	  done<<EOF
`find -L $dir ! -user root`		    
EOF
      fi
  done<<EOF
$REALPATH:
EOF

exit $RET

