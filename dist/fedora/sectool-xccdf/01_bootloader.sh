#!/bin/bash

# ----------------------------------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.                            #
# Written by Michel Samia <msamia@redhat.com>                 #
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>    #
# bootloader.sh                                               #
# ----------------------------------------------------------- #

RET=$XCCDF_RESULT_PASS

# should be XCCDF bound variable, TODO
GRUBCONF=/boot/grub/grub.conf

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

if [[ $UID -ne '0' ]]
then
    echo "You have to be logged as root to run this test!"
    exit ${XCCDF_RESULT_ERROR}
fi

check_file_perm ${GRUBCONF} 600 root:root 1 $E_BAD_PERMISSIONS "Bootloader configuration file"

if [[ "`egrep '^password' ${GRUBCONF} | wc -l`" == "0" ]]
then
	echo "${GRUBCONF} does not contain a password"
	echo "Please add the line 'password --md5 hash' to ${GRUBCONF}, where hash is output of grub-md5-crypt"	

	RET=$XCCDF_RESULT_FAIL
fi

exit $RET

