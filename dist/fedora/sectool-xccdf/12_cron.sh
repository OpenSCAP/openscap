#!/bin/bash
#
# Copyright (C) 2012 Red Hat, Inc.
# Author(s): Daniel Kopecek <dkopecek@redhat.com>
#

CRONTAB="/etc/crontab"
CRONTAB_PERM=644
CRONTAB_OWNER="root:root"

CRONDIRS=(/etc/cron.d /etc/cron.hourly /etc/cron.daily /etc/cron.weekly /etc/cron.monthly)

CRONDIRS_PERM=755
CRONDIRS_OWNER="root:root"

CRON_SHELL="/bin/bash"
CRON_PATH_DIRS=(/bin /sbin /usr/bin /usr/sbin)

CRON_CMD_OWNER="root:root"

shopt -s expand_aliases

alias const='declare -xr'
alias int='declare -xi'
alias var='declare -x'
alias array='declare -xa'

const ID_CRONTABPERM=1
const ID_CRONDIRPERM=2
const ID_DIFFCRONSHELL=3
const ID_MISSINGPATHDIR=4
const ID_UNEXPECTEDDIR=5
const ID_CRONWRONGOWNER=6

const SEDCMD="$(which sed)"
const STATCMD="$(which stat)"
const WHICHCMD="$(which which)"
const GREPCMD="$(which grep)"

int -r E_OK=0
int -r E_FAIL=1
int -r E_FATAL=2
int -r TRUE=0
int -r FALSE=1

function report {
    echo "$1: $3"
}

int RET=$XCCDF_RESULT_PASS

function update_RET {
    local new=$1

    case "$1" in
	"$XCCDF_RESULT_FAIL"|"$XCCDF_RESULT_PASS"|"$XCCDF_RESULT_INFORMATIONAL")
	    ;;
	*)
	    return;
    esac

    case "$RET" in
	"$XCCDF_RESULT_FAIL")
	    return
	    ;;
	"$XCCDF_RESULT_PASS")
	    RET=$new
	    ;;
	"$XCCDF_RESULT_INFORMAL")
	    if (( $new == $XCCDF_RESULT_FAIL )); then
		RET=$new
	    fi
	    ;;
	*)
	    return
	    ;;
    esac
}

function check_file_perm () {
    if [[ -a "${1}" ]]; then
	local CPERM=$(stat -c '%a' "${1}")
	
        if (( ${CPERM} != $2 )); then
	    if (( (8#${CPERM} | 8#${2}) == 8#${2} )); then 	
		if (( ${4} == 1 )); then
        	    report 'INFO' ${5} "Permissions on $(stat -c '%F' "${1}") \"${1}\" are more restrictive than required: ${CPERM} (${6:-uknown}, required persmissions are ${2})"
            	fi
            	return ${XCCDF_RESULT_FAIL}
	    else
            	if (( ${4} == 1 )); then
		    report 'WARNING' ${5} "Wrong permissions on $(stat -c '%F' "${1}") \"${1}\": ${CPERM} (${6:-unknown}, required permissions are ${2})"
            	fi
            	return ${XCCDF_RESULT_FAIL}
            fi
        fi
	
        if ! (stat -c '%U:%G' "${1}" | grep -q "${3}"); then
            if (( ${4} == 1 )); then
                report 'WARNING' ${5} "Wrong owner/group on $(stat -c '%F' "${1}"): \"${1}\" (${6:-unknown}, required owner/group is ${3})"
            fi
            return ${XCCDF_RESULT_FAIL}
        fi
	
        return ${XCCDF_RESULT_PASS}
    else
        if (( ${4} == 1 )); then
            report 'ERROR' ${5} "Missing file or directory: \"${1}\" (${6:-unknown})"
        fi
        return ${XCCDF_RESULT_FAIL}
    fi

    return ${XCCDF_RESULT_PASS}
}

# Check crontab perm
check_file_perm "${CRONTAB}" "${CRONTAB_PERM}" "${CRONTAB_OWNER}" 1 $ID_CRONTABPERM "tables for driving cron"
update_RET $?

# Check crondirs perm
while read dir; do
    check_file_perm "${dir}" "${CRONDIRS_PERM}" "${CRONDIRS_OWNER}" 1 $ID_CRONDIRPERM "cron directory"
    update_RET $?
done <<EOF
${CRONDIRS}
EOF

var NEWPATH

# Check vars in crontab
while read vars; do
    eval "${vars}"

    case "${VNAME}" in
	SHELL)
	    if [[ "${VAR_SHELL}" != "${CRON_SHELL}" ]]; then
		report 'WARNING' $ID_DIFFCRONSHELL "crontab variable SHELL is set to \"${VAR_SHELL}\". Expected value is \"${CRON_SHELL}\"."
		update_RET $XCCDF_RESULT_FAIL
	    fi
	    ;;
	PATH)
	    TMP=$IFS
	    IFS=$'\n'
	    EXP_DIRS=($((sort | uniq) <<< "${CRON_PATH_DIRS}"))
	    CUR_DIRS=($((tr ':' '\n' | sort | uniq) <<< "${VAR_PATH}"))
	    IFS=$TMP

	    for ((i=0; i < ${#EXP_DIRS[*]}; i++)); do
		if [[ -z "${CUR_DIRS[i]}" ]]; then
		    report 'WARNING' $ID_MISSINGPATHDIR "Missing directory \"${EXP_DIRS[i]}\" in PATH."
		    update_RET $XCCDF_RESULT_FAIL
		    break
		fi

		if [[ "${CUR_DIRS[i]}" != "${EXP_DIRS[i]}" ]]; then
		    if (( ${#CUR_DIRS[*]} < ${#EXP_DIRS[*]} )); then
		    	report 'WARNING' $ID_MISSINGPATHDIR "Missing directory \"${EXP_DIRS[i]}\" in PATH."
			update_RET $XCCDF_RESULT_FAIL
		    else
		    	report 'ERROR' $ID_UNEXPECTEDDIR "Unexpected directory \"${CUR_DIRS[i]}\" found in PATH. Expected is \"${EXP_DIRS[i]}\"."
			update_RET $XCCDF_RESULT_FAIL
		    fi
		    break;
		fi
	    done

	    NEWPATH="${VAR_PATH}"
	    ;;
    esac
done <<EOF
$(sed -n "s|^[[:space:]]*\([A-Z]*\)=\(.*\)[[:space:]]*$|VNAME=\"\1\" VAR_\1='\2'|p" "${CRONTAB}")
EOF

PATH="${NEWPATH}"

# Check crontab records
while read vars; do
    eval "${vars}"

    if [[ -z "${CMD}" ]]; then
	continue
    fi

    case "${CMD}" in
	/*)
	    CMDPATH="${CMD}"
	    ;;
	*)
	    CMDPATH="$(${WHICHCMD} "${CMD}")"
	    ;;
    esac

    CMDOWNER="$(${STATCMD} -c "%U:%G" "${CMDPATH}")"

    if ! (echo "${CMDOWNER}" | ${GREPCMD} -q "${CRON_CMD_OWNER}"); then
	report 'WARNING' $ID_CRONWRONGOWNER "Wrong owner/group \"${CMDOWNER}\" on \"${CMDPATH}\". Expected is \"${CRON_CMD_OWNER}\"."
	update_RET $XCCDF_RESULT_FAIL
    fi
done <<EOF
$(${SEDCMD} -n -e "s|^\([0-9\*,-]\+\)[[:space:]]\+\([0-9\*,-]\+\)[[:space:]]\+\([0-9\*,-]\+\)[[:space:]]\+\([0-9\*,-]\+\)[[:space:]]\+\([0-9\*,-]\+\)[[:space:]]\+\([a-zA-Z0-9_-]*\)[[:space:]]\+\([^;\$[:space:]]\+\)[[:space:]]*\(.*\)$|MIN=\"\1\" HRS=\"\2\" DAYM=\"\3\" DAYW=\"\4\" MON=\"\5\" USER=\"\6\" CMD='\7' CMDARGS='\8'|p" "${CRONTAB}")
EOF

exit ${RET}
