#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
# disc_usage.sh                                               #
# more info in disc_usage.dsc                                 #
# ----------------------------------------------------------- #

# secTool script testing directories from standarad hierarchy for presence, permissions and owner
# for more info about FHS see http://www.pathname.com/fhs/pub/fhs-2.3.html


E_DOESNT_EXIST=1
E_WRONG_OWNER=2
E_WRONG_PERM=3

dirs="/ 555
/bin 555
/boot 555
/dev 755
/etc 755
/home 755
/lib 555
/media 755
/mnt 755
/opt 755
/root 550
/sbin 555
/srv 755
/tmp 1777
/usr 755
  /usr/bin 555
  /usr/sbin 555
  /usr/include 755
  /usr/lib 555
  /usr/share 755
  /usr/src 755
  /usr/local 755
/var 755
  /var/lock 775
  /var/log 755
  /var/mail 777
  /var/run 755
  /var/spool 755
  /var/spool/mail 775
  /var/tmp 1777
$DIRS"

ret=$XCCDF_RESULT_PASS

while read dir perm
do
    #echo "dir: $dir   perm: $perm"
    [ "$dir" == "" ] && continue

    # exists?
    if ! [[ -d $dir ]]
    then
        echo "Directory $dir doesn't exist! Please create it."

        ret=$XCCDF_RESULT_FAIL
    else
	# owner
	if [[ "`stat -c '%U' $dir`" != "root" ]]
	then
	    echo "Directory $dir has wrong owner. Change the owner to root."

            ret=$XCCDF_RESULT_FAIL
	fi

	if [[ "`stat -c '%a' $dir`" != "$perm" ]]
	then
	    echo "Directory $dir has wrong permissions! Change the permissions to $perm."

            ret=$XCCDF_RESULT_FAIL
	fi
    fi
done <<EOF
$dirs
EOF

exit $ret

