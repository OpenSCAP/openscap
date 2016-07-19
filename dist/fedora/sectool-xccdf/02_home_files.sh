#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# home_files.sh                                               #
# more info in home_files.dsc                                 #
# ----------------------------------------------------------- #

RET=$XCCDF_RESULT_PASS

# test of permissions and owner of some interesting files and dirs in ~ 
# (.ssh, private keys, .bashrc, .bash_profile...)

#constants
passwd=/etc/passwd
useradd=/etc/default/useradd
default_homedir=/home

# files that should not be owned by someone other than the home directory owner, or readable
# even read permission can be dangerous for these files
NO_READ_NO_WRITE_FILES="\
    .netrc \
    .rhosts \
    .shosts \
    .Xauthority \
    .gnupg/secring.gpg \
    .pgp/secring.pgp \
    .ssh/identity \
    .ssh/id_dsa \
    .ssh/id_rsa \
    .ssh/random_seed \
    .pgp/randseed.bin"

# files that should not be owned by someone other than the home directory owner, or writeable
# write permission to these files is dangerous
NO_WRITE_FILES="\
    .bashrc \
    .bash_profile \
    .bash_login \
    .bash_logout \
    .cshrc \
    .emacs \
    .exrc \ 
    .forward \
    .gdbrc \
    .klogin \
    .login \ 
    .logout \
    .profile \
    .tcshrc \
    .fvwmrc \
    .inputrc \
    .kshrc \
    .nexrc \
    .screenrc \
    .ssh \
    .ssh/config \
    .ssh/authorized_keys \
    .ssh/environment \
    .ssh/known_hosts \
    .ssh/rc \
    .twmrc \
    .vimrc \
    .viminfo \
    .xsession \
    .xinitrc \
    .Xdefaults \
    .zshenv \
    .zprofile \
    .zshrc \
    .zlogin \
    .zlogout"

if [[ $UID -ne '0' ]]
then
    echo "You have to be logged as root to run this test!"
    exit $XCCDF_RESULT_ERROR
fi

####  tries to find default home directory ####
if [[ `egrep 'HOME=.+' $useradd|wc -l` -eq "1" ]]
then
    homedir=`egrep 'HOME=.+' $useradd|awk -F= '{print $2}'`
    #DEBUGMSG "Detected $homedir as users' home directory"
else
    echo "Could not detect users' home directory in $useradd, the default will be used" 
    homedir=$defaulthomedir # not found, uses standard /home
fi

#### test permissions of sensitive files  ####
i=2 # passwd line number

# go thru users
while read line # of passwd
do
    i=$[ i+1 ]
    if [[ `echo $line| egrep ':/sbin/nologin$'|wc -l` -eq "0" ]] 
    then 
	# user is 'normal' :-)
	line_home=`echo $line | awk -F: '{print $6}'`
	line_username=`echo $line | awk -F: '{print $1}'`
	line_userID=`echo $line | awk -F: '{print $3}'`

	if [[ -d $line_home ]]
	then
	    for files in noreadnowrite nowrite
	    do
		if [[ $files == "noreadnowrite" ]]
		then
			pole=$NO_READ_NO_WRITE_FILES
		elif [[ $files == "nowrite" ]]
		then
			pole=$NO_WRITE_FILES
		fi

		# go thru files
		for tested_file in $pole
		do
			fullpath=$line_home/$tested_file

			if ! [[ -f $fullpath ]] 
			then
				continue
			fi

			perm=`stat -c '%A' $fullpath`
			ownerID=`stat -c '%u' $fullpath`
			ownerName=`stat -c '%U' $fullpath`

			# ownership
			if [ $ownerID -ne $line_userID ]
			then
				echo "\"$line_username\"'s file $tested_file is owned by user \"$ownerName\"!"
			fi

			# user - do we want to warn about this???
			if [[ ${perm:1:1} != 'r' ]]
			then
				echo "User \"$line_username\" can't read his $tested_file!"
				[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
			fi

			#group
			if [[ ${perm:4:1} != '-' ]] && [[ $files == "noreadnowrite" ]]
			then
				echo "User \"$line_username\" allows users from his group to read his $tested_file!"
				[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
			fi
			if [[ ${perm:5:1} != '-' ]]
			then
				echo "User \"$line_username\" allows users from his group to write to his $tested_file!"
				RET=$XCCDF_RESULT_FAIL
			fi

			# others
			if [[ ${perm:7:1} != '-' ]] && [[ $files == "noreadnowrite" ]]
			then
				echo "User \"$line_username\" allows other users to read his $tested_file!"
				RET=$XCCDF_RESULT_FAIL
			fi
			if [[ ${perm:8:1} != '-' ]]
			then
				echo "User \"$line_username\" allows other users to write to his $tested_file!"
				RET=$XCCDF_RESULT_FAIL
			fi
		done
	    done
	fi
    fi
done<<EOF
`cat $passwd`
EOF

exit $RET

