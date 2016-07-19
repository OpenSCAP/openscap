#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
#                                                             #
# tcp_wrappers.sh                                             #
# more info in tcp_wrappers.dsc                               #
# ----------------------------------------------------------- #

CHECK="/usr/bin/* /usr/sbin/*"
ALLOW='/etc/hosts.allow'
DENY='/etc/hosts.deny'

# filter binaries which use specified library
# stdin: list of paths to binaries
# stdout: filtered list
# usage: filter_lib libname
function filter_lib {
	while read prog; do
		ldd "$prog" 2>/dev/null | awk '{ print $1 }' \
			| sed -e 's/\.so\(\.[0-9]\+\)\?//g' -e 's/^.*\///g' \
			| grep -F "$1" >/dev/null && echo "$prog"
	done
}

# get list of applications, which are linked with libwrap
apps=`ls -1 $CHECK | filter_lib libwrap | grep -v config | xargs -n1 basename`
ps="`ps -eo comm=`"

while read app
do

	# TODO: it would be better to grep it with sth like "^[^:#]*$app|^ALL *:" to work with lists of services on one line...

	# if the application is not running, we will not cry about it..
	echo "$ps" | grep "$app" >/dev/null || continue

	#report 'INFO' 1 "App: $app"

	egrep "^$app|^ALL *:" $ALLOW > /dev/null
	isInAllow=$[ 1 - $? ]

	egrep "^$app|^ALL *:" $DENY > /dev/null
	isInDeny=$[ 1 - $? ]

	if [ $isInAllow -ne 1 ] && [ $isInDeny -ne 1 ]
	then
		echo "Application $app is using tcp wrappers, but does not specify any restrictions in $ALLOW nor in $DENY"
		echo "Please consider restricting access to this service i.e. according to IP address ranges"
		RET=$XCCDF_RESULT_FAIL
	fi

done<<EOF
$apps
EOF

exit $RET

