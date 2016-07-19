#!/bin/bash
#
# Copyright (C) 2012 Red Hat, Inc.
# Author(s): Daniel Kopecek <dkopecek@redhat.com>
#

OVPN_USER="openvpn"
OVPN_GROUP="openvpn"
OVPN_CONFIG_PERM=755
OVPN_CONFIG_OWNER="root:root"
OVPN_CONFIGDIR_PERM=755
OVPN_CONFIGDIR_OWNER="root:root"
OVPN_PRIVKEY_PERM=600
OVPN_PRIVKEY_OWNER="root:root"
OVPN_CERT_PERM=644
OVPN_CERT_OWNER="root:root"
#OVPN_CHECK_SHA256=1
#OVPN_SHA256_LIST="sshd.sum"
#OVPN_SAVE_SHA256_CHANGES=1
OVPN_DEFAULT_KEYS="/etc/openvpn/client.key /etc/openvpn/server.key"
OVPN_DEFAULT_DIR="/etc/openvpn"
OVPN_DEFAULT_CONF="/etc/openvpn/openvpn.conf"
PARANOID=1

shopt -s expand_aliases

alias const='declare -xr'
alias int='declare -xi'
alias var='declare -x'
alias array='declare -xa'

int -r E_OK=0
int -r E_FAIL=1
int -r E_FATAL=2
int -r TRUE=0
int -r FALSE=1

const E_NOFILE=13
const E_NORECORD=14
const E_NOLIST=15
const E_DIFF=16
const E_NEWLIST=17

const ID_CHKLIST_FILECHANGED=1
const ID_CHKLIST_FILENOTFOUND=2
const ID_CHKLIST_NORECORD=3
const ID_CHKLIST_NOTFOUND=4
const ID_CKHLIST_NEW=5
const ID_KEYNOTENCRYPTED=6
const ID_OVPN_CONFDIRPERM=7
const ID_OVPN_WRONGPGRP=8
const ID_OVPN_WRONGPUSER=9
const ID_OVPN_PRIVKEYPERM=10

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

function rel2abs () {
    local DIR="$(dirname  "${1}")"
    local FIL="$(basename "${1}")"
    local OLD="$(pwd)"
    cd "${DIR}" && echo -n "$(pwd)/${FIL}" || echo "${1}"
    cd "${OLD}"
}

function uid2user () {
    getent passwd | sed -n "s/^\([^:]*\):[^:]*:${1}:.*/\1/p"
}

function gid2user () {
    getent group | sed -n "s/^\([^:]*\):[^:]*:${1}:.*/\1/p"
}

function report {
    echo "$1: $3"
}

function canLogIn {
    if [[ "$(getent passwd "${1}" | cut -d: -f 7)" != "/sbin/nologin" ]]; then 
	password="$(getent shadow "${1}" | cut -d: -f 2)"
	# length of passwd - very short means invalid password and disabled account
	if (( ${#password} < 13 )); then
	    return ${E_FAIL}
	else
	    return ${E_OK}
	fi
    else
	return ${E_FAIL}
    fi
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

function check_env () {
    PROC="`mount | sed -n 's/.*on\s\(.*\)\stype proc.*/\1/p'`"
    if [[ -d "${PROC}" ]]; then
	return ${E_OK}
    else
	if `mount -t proc &> /dev/null`; then
	    if check_env; then
		return ${E_OK}
	    else
		return ${E_FAIL}
	    fi
	fi
    fi
    return ${E_FAIL}
}

function proc_getopt () { # 1 - cmdline, 2 - opt
    cat ${1} | tr '\0' '\t' | sed -n "s/.*${2}\t\([^\t]*\)\t.*/\1/p" && echo
}

function proc_getenv () {
    cat ${1} | tr '\0' '\n' | sed -n "s/^${2}=\(.*\)$/\1/p"
}

# <proc_statuc>
function proc_getuid () {
    sed -n 's/^Uid:\s\([[:digit:]]*\).*/\1/p' "${1}"
}

# <proc_status>
function proc_getgid () {
    sed -n 's/^Gid:\s\([[:digit:]]*\).*/\1/p' "${1}"
}

# <file> <list>
function sha256cmp () {
    if [[ ! -f "${1}" ]]; then
	return ${E_NOFILE}
    else
	local NEW="`sha256file \"${1}\"`"
    fi

    if [[ ! -f "${2}" ]]; then
	echo ${NEW}
	touch "${2}" && return ${E_NEWLIST}
	return ${E_NOLIST}
    fi

    local OLD=$(grep "SHA256(${1})" "${2}" 2> /dev/null)

    if [[ -n "${OLD}" ]]; then
	if [[ "${NEW}" == "${OLD}" ]]; then
	    return ${E_OK}
	else
	    echo ${NEW}
	    return ${E_DIFF}
	fi
    else
	echo ${NEW}
	return ${E_NORECORD}
    fi
}

## <file> <list> <save_flag> [<id>]
#function check_sha256 () {
#    local NEW
# 
#    NEW="`sha256cmp \"${1}\" \"${2}\"`"
#
#    case "$?" in
#	"${E_NEWLIST}")
#	    echo ${NEW} > "${2}"
#	    report 'WARNING' $ID_CKHLIST_NEW      "Created new SHA256 list."
#	    ;;
#	"${E_NOLIST}")
#	    report 'WARNING' $ID_CHKLIST_NOTFOUND "SHA256 list not found."
#	    ;;
#	"${E_OK}")
#	    return ${E_OK}
#	    ;;
#	"${E_DIFF}")
#	    report 'WARNING' $ID_CHKLIST_FILECHANGED "File \"${1}\" changed!"
#	    if (( ${3} == 1 )); then
#		grep -v "SHA256(${1})" "${2}" > "${2}.new"
#		echo ${NEW} >> "${2}.new"
#		mv "${2}.new" "${2}"
#	    fi
#	    return ${E_DIFF}
#	    ;;
#	"${E_NORECORD}")
#	    report 'WARNING' $ID_CHKLIST_NORECORD "No SHA256 record for file \"${1}\""
#	    if (( ${3} == 1 )); then
#		echo ${NEW} >> "${2}"
#	    fi
#	    return ${E_NORECORD}
#	    ;;
#	"${E_NOFILE}")
#	    report 'WARNING' $ID_CHKLIST_FILENOTFOUND "File \"${1}\" not found."
#	    return ${E_NOFILE}
#	    ;;
#	*)
#	    # FIXME: ...
#	    return ${E_FAIL}
#	    ;;
#    esac
#}

# __ovpn_getconf sshd_config HostKey
function __ovpn_getconf () {
    sed -n -e "s/^[^#]*[\s]*${2}\s\(.*\)/\1/p" ${1}
}

# __ovpn_getoptconf <cmdline> HostKey
function __ovpn_getoptconf {
    case "${2}" in
	-*)
	    proc_getopt "${1}" "${2}"
	    ;;
	*)
	    proc_getopt "${1}" "--${OPT}"
    esac
}

function __check_ovpn_config () {
    local PROCDIR=${1}
    local CONFFILE=${2}
    local CONFDIR="$(dirname ${CONFFILE})"
    local CLIENT=0

    # check permissions dir + config
    check_file_perm "${CONFDIR}"  ${OVPN_CONFIGDIR_PERM} ${OVPN_CONFIGDIR_OWNER} 1 $ID_OVPN_CONFDIRPERM "OpenVPN configuration directory"
    update_RET $?
    check_file_perm "${CONFFILE}" ${OVPN_CONFIG_PERM}    ${OVPN_CONFIG_OWNER}    1 $ID_OVPN_CONFDIRPERM "OpenVPN configuration file"
    update_RET $?
    if [[ -n "$(__ovpn_getconf "${CONFFILE}" client)" || -n "$(__ovpn_getoptconf "${PROCDIR}/cmdline" client)" || \
	  -n "$(__ovpn_getconf "${CONFFILE}" tls-client)" || -n "$(__ovpn_getoptconf "${PROCDIR}/cmdline" tls-client)" ]]; then
	CLIENT=1
    fi

    # resolve keyfile names + permission check
    # keyfiles from config

    for keyfile in `(__ovpn_getconf "${CONFFILE}" key; __ovpn_getoptconf "${PROCDIR}/cmdline" key) | sort | uniq`; do
	case "${keyfile}" in
	    /*)
		keyfile=$(rel2abs ${keyfile})
		;;
	    *)
	        CD="$(__ovpn_getconf "${CONFFILE}" cd)"
		if [[ -z "${CD}" ]]; then
		    CD="$(__ovpn_getoptconf "${PROCDIR}/cmdline" cd)"
		fi
	        keyfile=$(rel2abs "${CD}/${keyfile}")
		;;
	esac

	check_file_perm "${keyfile}"     ${OVPN_PRIVKEY_PERM} ${OVPN_PRIVKEY_OWNER}  1 $ID_OVPN_PRIVKEYPERM "OpenVPN private key"
	update_RET $?
	#check_sha256    "${keyfile}"     "${TDATA_DIR}/${OVPN_SHA256_LIST}" "${OVPN_SAVE_SHA256_CHANGES}"

	if [[ -z "$(grep ENCRYPTED "${keyfile}")" && ${CLIENT} == 1 ]]; then
	    report 'WARNING' $ID_KEYNOTENCRYPTED "Your OpenVPN private key \"${keyfile}\" IS NOT encrypted."
	    update_RET $XCCDF_RESULT_FAIL
	fi
    done

    # TODO: options check
    return ${E_OK}
}

function CHK_openvpn () {
    local PROCDIR=${2}
    local PKGNAME=${1}
    local CURRENT_CONF=$(proc_getopt "${PROCDIR}/cmdline" --config)
    local DEFAULT_CONF="${OVPN_DEFAULT_DIR}/openvpn.conf"
    local PUSER=$(uid2user `proc_getuid ${PROCDIR}/status`)
    local PGROUP=$(gid2user `proc_getgid ${PROCDIR}/status`)

    if [[ ${PUSER}  != ${OVPN_USER} ]]; then
	report 'WARNING' $ID_OVPN_WRONGPUSER "Wrong process owner (${PUSER}) - required is ${OVPN_USER}"
	update_RET $XCCDF_RESULT_FAIL
    fi

    if [[ ${PGROUP} != ${OVPN_GROUP} ]]; then
	report 'WARNING' $ID_OVPN_WRONGPGRP "Wrong process group (${PGROUP}) - required is ${OVPN_GROUP}"
	update_RET $XCCDF_RESULT_FAIL
    fi

    unset PUSER
    unset PGROUP

    if [[ -n "${CURRENT_CONF}" ]]; then
	case "${CURRENT_CONF}" in
	    /*)
		CURRENT_CONF=$(rel2abs "${CURRENT_CONF}")
		;;
	    *)
		CURRENT_CONF=$(rel2abs "$(proc_getenv ${PROCDIR}/environ PWD)/${CURRENT_CONF}")
		;;
	esac

	if [[ "${CURRENT_CONF}" != "${DEFAULT_CONF}" ]]; then
	    # check current
	    if [[ -f "${CURRENT_CONF}" ]]; then
		__check_ovpn_config "${PROCDIR}" "${CURRENT_CONF}"
	    fi
	fi
    fi

    # check default
    if [[ -f "${DEFAULT_CONF}" ]]; then
	__check_ovpn_config "${PROCDIR}" "${DEFAULT_CONF}"
    fi
}

########################################

check_env || exit ${E_FAIL}

for PID in $(netstat -nltup | sed -n 's|^.*[[:space:]]\+\([0-9]\{1,5\}\)/openvpn|\1|p' | sort | uniq); do
    report 'INFO' 0 "Checking openvpn configuration (PID=${PID})"
    CHK_openvpn "openvpn" "${PROC}/${PID}/"
done

exit ${RET}
