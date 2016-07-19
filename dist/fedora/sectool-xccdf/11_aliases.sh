#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
#                                                             #
# aliases.sh                                                  #
# ----------------------------------------------------------- #

# far from perfect but mostly works
STRING_RE="'[^']*'|\"(\\\\.|[^\"])*\""

RET=$XCCDF_RESULT_PASS

# test all defined aliases
while read line
do
	name="`echo $line | awk -F= '{ print $1 }'`"
	value="`echo $line | awk -F= '{ print $2 }' | sed "s/^'//" | sed "s/'$//"`" # all what is ritht from '=' , without border apostrophs

	# todo: check all commands, since one alias can contain more, separated i.e. by ';', '&&' and so on

	#value="`"echo $value" | sed 

	# take a first command..
	command="`echo $value | awk '{ print $1 }' `"
	if [[ "$command" == "." ]] || [[ "$command" == "source" ]]
	then
		included_file="`echo $value | awk '{ print $2 }' `"
		is_command="0"
	else
		is_command="1"
	fi

	if [[ "$is_command" == "1" ]]
	then
		fullpath="`which $command 2>&1`"
		commandWithoutPath="`basename $command`"	

		# does is exist?
		if [[ "`echo "$fullpath" |grep -c "no $commandWithoutPath in"`" == "1" ]]
		then
			if [[ "$command" != "alias" ]] # fedora uses something strange for 'which'..
			then
				echo "Alias '$name' contains command '$command', which was not found"
				[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
			fi
		else

			# is this file secure? (means not world/group writable + owned by root
			perm="`stat -L -c '%A' $fullpath`"
			if [[ ${perm:5:1} != '-' ]]
			then
				echo "Alias '$name' contains command '$command', which is is group writable"
				echo "Please unset it the group writing bit"
				RET=$XCCDF_RESULT_FAIL
			fi


			if [[ ${perm:8:1} != '-' ]]
			then
				echo "Alias '$name' contains command '$command', which is is world writable"
				echo "Please unset it the world writing bit"
				RET=$XCCDF_RESULT_FAIL
			fi

			owner="`stat -c '%U' $fullpath`"
			if [[ "$owner" != "root" ]]
			then
				echo "Alias '$name' contains command '$command', which is not owned by root"
				echo "Please chown this file to root:root"
				RET=$XCCDF_RESULT_FAIL
			fi
		fi

	fi

	# it is an included file
	if [[ "$is_command" == "0" ]]
	then
		# does it exist? 
		if ! [[ -f $included_file ]]
		then
			echo "Alias '$name' includes file '$included_file', which does not exist"
			[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
		else

			# is this file secure? (means not world/group writable + owned by root
			perm="`stat -L -c '%A' $included_file`"
			if [[ ${perm:5:1} != '-' ]]
			then
				echo "Alias '$name' includes file '$included_file', which is is group writable"
				echo "Please unset it the group writing bit"
				RET=$XCCDF_RESULT_FAIL
			fi


			if [[ ${perm:8:1} != '-' ]]
			then
				echo "Alias '$name' includes file '$included_file', which is is world writable"
				echo "Please unset it the world writing bit"
				RET=$XCCDF_RESULT_FAIL
			fi

			owner="`stat -c '%U' $fullpath`"
			if [[ "$owner" != "root" ]]
			then
				echo "Alias '$name' includes file '$included_file', which is not owned by root"
				echo "Please chown this file to root:root"
				RET=$XCCDF_RESULT_FAIL
			fi
		fi
	fi
done<<EOF
`bash -l -c 'alias' | grep '^alias ' | sed 's/^alias//'`
EOF

exit $RET

