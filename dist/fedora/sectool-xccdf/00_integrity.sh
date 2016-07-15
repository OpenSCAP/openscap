#!/bin/bash

# -------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.
# Written by Dan Kopecek <dkopecek@redhat.com>
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>

RET=$XCCDF_RESULT_PASS

# For now we are storing these hardcoded in here,
# but we would like to pass them as XCCDF bound variables in the future!
CMDPATH="/bin:/sbin:/root/bin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin"
RPMVERIFY="/usr/bin/rpmverify"
COREPKG="coreutils"
DIRECTORY_LIST="/bin
		/sbin
		/lib
		/usr/bin
		/usr/sbin
		/usr/lib
		/usr/libexec
		/tmp
		/proc
		/var"

function check_dirs () {
    while read dir; do
	if [[ ! -d "${dir}" ]]; then
	    echo "Directory \"${dir}\" does not exist!"
	    RET=${XCCDF_RESULT_FAIL}
	fi
    done <<EOF
${DIRECTORY_LIST}
EOF
}

function check_cmds () {
    OUTPUT="$(${RPMVERIFY} --noscript --nomtime -f "${COREPKG}" 2> /dev/null)"
    if (( $? != 0 )); then
	if [[ -n "$(echo -e "${OUTPUT}" | sed -n 's|^[.A-Z0-9]\{8\}[[:space:]]\{1,\}[^c][[:space:]]\{1,\}.*$|&|p')" ]]; then
	    echo "Verify failed for package \"${COREPKG}\"!"
	    RET=$XCCDF_RESULT_FAIL
	fi
    fi

    while read path; do    
	if [[ "$(stat -c '%A' "${path}" | cut -c 1)" != "l" ]]; then 
	    COMMANDNAME="$(basename "${path}")"
	    COMMANDPATH="$(dirname  "${path}")"
	    COMMANDPATHS="$(echo "${CMDPATH}" | tr ':' ' ' | sed -e "s|[[:space:]]${COMMANDPATH}||g" -e "s|^${COMMANDPATH}[[:space:]]||g")"

	    unset DUPLICATES
	    I=0

	    while read duplicate; do
		if [[ -n "${duplicate}" ]]; then
		    if [[ "$(stat -c '%A' "${duplicate}" | cut -c 1)" != "l" ]]; then
			DUPLICATES[$I]="${duplicate}"
			I=$(($I + 1))
		    else
			DEST="$(readlink -e "${duplicate}" 2> /dev/null)"
			if [[ "${DEST}" != "${path}" ]]; then
			    DUPLICATES[$I]="${duplicate}"
			    I=$(($I + 1))
			fi	
		    fi
		fi
	    done <<EOF
`find ${COMMANDPATHS} -maxdepth 1 -mindepth 1 -name "${COMMANDNAME}" 2> /dev/null`
EOF
	    if (( $I > 0 )); then
		echo "Command \"${COMMANDNAME}\" (${path}) has duplicates in these locations: ${DUPLICATES[*]}!"
		RET=$XCCDF_RESULT_FAIL
	    fi
	fi
    done <<EOF
`rpm -ql "${COREPKG}" | grep bin/`
EOF
}

# --- Main ----------------------- #

PATH="${CMDPATH}"

check_dirs 
check_cmds 

exit $RET

# --- EOF ------------------------ #
