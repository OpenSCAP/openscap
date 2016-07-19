#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
# nfs.sh                                                      #
# more info in nfs.dsc                                        #
# ----------------------------------------------------------- #

# todo: remove comments first

EXPORTS="/etc/exports"

W_EXPORTS_NOT_FOUND=1
W_RW=2

if ! [[ -f $EXPORTS ]]
then
	echo 'WARNING' $W_EXPORTS_NOT_FOUND 'File $EXPORTS was not found!'
	exit $XCCDF_RESULT_FAIL
fi

ret=$XCCDF_RESULT_PASS

cat $EXPORTS | sed 's/ \+/\t/g' |sed 's/\t\+/\t/g' |  # spaces and tabs to one tab
{
	linenr=0

	while read line
	do
		linenr=$[ linenr + 1 ]
		lineWithoutComments=`echo "$line" | sed 's/#.*//' `

		numOfFields=`echo $lineWithoutComments | awk '{ print NF }'`
		dir=`echo "$lineWithoutComments" | awk '{ print $1 }'`
#		report 'WARNING' 1 "dir: $dir; numOfFields: $numOfFields"

		for((i=2; i<=numOfFields; i++))
		do
			machine=`echo "$lineWithoutComments" | cut -f $i`
			params=`echo ${machine} | sed 's/^[^(]*(//' | sed 's/)$//' `
#			report 'WARNING' 1 "    mach: $machine, params: $params, i: $i"
			machine="`echo $machine | sed 's/(.*//'`"
			if echo $params | egrep '(^|,)rw(,|$)' >/dev/null
			then
				echo 'WARNING' $W_RW "$EXPORTS: line $linenr: Directory $dir is exported to $machine with enabled write permission"
				ret=$XCCDF_RESULT_FAIL
			fi
		done	
#		IFS=$OLDIFS	
	done
}

exit $ret

