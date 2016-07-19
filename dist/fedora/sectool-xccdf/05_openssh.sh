#!/bin/bash
#
# Copyright (C) 2012 Red Hat, Inc.
# Author(s): Daniel Kopecek <dkopecek@redhat.com>
#

if [[ $UID -ne '0' ]]
then
    echo "You have to be logged as root to run this test!"
    exit ${XCCDF_RESULT_ERROR}
fi

SSHD_USER="root"
SSHD_GROUP="root"
SSHD_CONFIG_PERM=600
SSHD_CONFIG_OWNER="root:root"
SSHD_CONFIGDIR_PERM=755
SSHD_CONFIGDIR_OWNER="root:root"
SSHD_PRIVKEY_PERM=600
SSHD_PRIVKEY_OWNER="root:root"
SSHD_PUBKEY_PERM=644
SSHD_PUBKEY_OWNER="root:root"
SSH_KNOWN_HOSTS_REQUIRE=0
SSH_KNOWN_HOSTS_PERM=644
SSH_KNOWN_HOSTS_OWNER="root:root"
SSHD_PRIVSEP_DIR_REQUIRE=1
SSHD_PRIVSEP_DIR_PERM=711
SSHD_PRIVSEP_DIR_OWNER="root:root"
SSHD_PRIVSEP_DIR="/var/empty/sshd"
SSHD_PRIVSEP_DIR_EMPTY=0
#SSHD_CHECK_SHA256=1
#SSH_CHECK_SHA256=1
#SSHD_SHA256_LIST="sshd.sum"
SSHD_SAVE_SHA256_CHANGES=0
SSHD_DEFAULT_KEYS="/etc/ssh/ssh_host_dsa_key
		   /etc/ssh/ssh_host_key
		   /etc/ssh/ssh_host_rsa_key"
PARANOID=1

# --- OpenSSH default values --- #

SSHD_defined_vars="PermitRootLogin ChallengeResponseAuthentication UsePAM PermitUserEnvironment StrictModes UsePrivilegeSeparation ClientAliveCountMax ClientAliveInterval"

# Examples
# version = 4.7p1: SSHD_47p1_PermitRootLogin="yes"
# version = 4.x  : SSHD_4_PermitRootLogin="yes"
# version = all  : SSHD__PermitRootLogin="yes"

# 4.x on F8
#SSHD_4_AllowGroups=""
#SSHD_4_AllowUsers=""
SSHD_4_PermitRootLogin="yes"
SSHD_4_ChallengeResponseAuthentication="yes"
SSHD_4_UsePAM="no"
SSHD_4_PermitUserEnvironment="no"
SSHD_4_StrictModes="yes"
SSHD_4_UsePrivilegeSeparation="yes"
SSHD_4_ClientAliveCountMax=3
SSHD_4_ClientAliveInterval=0

# 5.x on F9
SSHD_5_PermitRootLogin="yes"
SSHD_5_ChallengeResponseAuthentication="yes"
SSHD_5_UsePAM="no"
SSHD_5_PermitUserEnvironment="no"
SSHD_5_StrictModes="yes"
SSHD_5_UsePrivilegeSeparation="yes"
SSHD_5_ClientAliveCountMax=3
SSHD_5_ClientAliveInterval=0

###########################################################

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

# --- Error IDs ------- #
const ID_CHKLIST_FILECHANGED=1
const ID_CHKLIST_FILENOTFOUND=2
const ID_CHKLIST_NORECORD=3
const ID_CHKLIST_NOTFOUND=4
const ID_CHKLIST_NEW=5
const ID_CLIALIVEINTLONG=6
const ID_CLIALIVEINTZERO=7
const ID_PRIVSEPDIR_NOTEMPTY=8
const ID_SSHD_ACCESSNOTRESTRICTED=9
const ID_SSHD_CHRESPAMDISABLED=10
const ID_SSHD_CLIALIVEMAXHIGH=11
const ID_SSHD_CLIALIVEMAXZERO=12
const ID_SSHD_CONFDIRPERM=13
const ID_SSHD_KNOWNDBPERM=14
const ID_SSHD_OPTPRIVKEYPERM=15
const ID_SSHD_OPTPUBKEYPERM=16
const ID_SSHD_PRIVKEYPERM=17
const ID_SSHD_PRIVSEPDISABLED=18
const ID_SSHD_PRIVSEPPERM=19
const ID_SSHD_PUBKEYPERM=20
const ID_SSHD_REMOTEROOTFORCEDONLY=21
const ID_SSHD_REMOTEROOTNOTFALSE=22
const ID_SSHD_REMOTEROOTPUBKEY=23
const ID_SSHD_REMOTEROOTTRUE=24
const ID_SSHD_STRICTMODESDISABLED=25
const ID_SSHD_USERENVENABLED=26
const ID_SSHD_WRONGPGRP=27
const ID_SSHD_WRONGPUSER=28
const ID_FIRSTRUN=29
const ID_FILE_AUTHKEYS2=30
const ID_FILE_KNOWN2=31
const ID_FILE_TWOAUTHKEYS=32
const ID_FILE_TWOKNOWNHOSTS=33
const ID_SSHD_DEPPROTOVER=34

array CHECKED_CONFIGS

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

function LoadDefaults () {
    local PREFIX="${1}"
    local VER="${2}"
    local value=""

    for var in $(eval "echo \$${PREFIX}_defined_vars"); do
	for ((i = $(echo ${VER} | wc -c); i >= 0; i--)); do
	    value="$(eval "echo \$${PREFIX}_${VER:0:i}_${var}")"

	    if [[ -n "${value}" ]]; then
		export ${var}="${value}"
		break;
	    fi
	done
    done
}

function notchecked () {
    for ((i=0; i < ${#CHECKED_CONFIGS[*]}; i++)); do
	if [[ "${1}" == "${CHECKED_CONFIGS[$i]}" ]]; then
	    return ${E_FAIL}
	fi
    done

    return ${E_OK}
}

function addchecked () {
    CHECKED_CONFIGS[${#CHECKED_CONFIGS[*]}]="${1}"
}

# TODO: get rid of this function
function check_env () {
    PROC="$(mount | sed -n 's/.*on\s\(.*\)\stype proc.*/\1/p')"
    if [[ -d "${PROC}" ]]; then
	return ${E_OK}
    else
	if mount -t proc &> /dev/null; then
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
    (tr '\0' '\t' | sed -n "s/.*${2}\t\([^\t]*\)\t.*/\1/p" && echo) < "$1"
}

function proc_getenv () {
    (tr '\0' '\n' | sed -n "s/^${2}=\(.*\)$/\1/p") < "$1"
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
	local NEW="$(sha256file "${1}")"
    fi

    if [[ ! -f "${2}" ]]; then
	echo ${NEW}
	touch "${2}" && return ${E_NEWLIST}
	return ${E_NOLIST}
    fi

    local OLD="$(grep "SHA256(${1})" "${2}" 2> /dev/null)"

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

# <file> <perm> <user:group> [0|1 <id> <desc>]
# 1 - verbose
# 0 - quiet

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

## <file> <list> <save_flag> <msg_on>
#function check_sha256 () {
#    local NEW="$(sha256cmp "${1}" "${2}")"
#    local L_RET=$XCCDF_RESULT_PASS
#
#    case "$?" in
#	"${E_NEWLIST}")
#	    echo ${NEW} > "${2}"
#	    if (( ${4} == 1 )); then
#		report 'WARNING' $ID_CHKLIST_NEW "Created new SHA256 list." 
#	    fi
#	    L_RET=$XCCDF_RESULT_INFORMATIONAL
#	    ;;
#	"${E_NOLIST}")
#	    if (( ${4} == 1 )); then
#		report 'WARNING' $ID_CHKLIST_NOTFOUND "SHA256 list not found."
#	    fi
#	    L_RET=$XCCDF_RESULT_INFORMATIONAL
#	    ;;
#	"${E_OK}")
#	    ;;
#	"${E_DIFF}")
#	    if (( ${4} == 1 )); then
#		report 'WARNING' $ID_CHKLIST_FILECHANGED "File \"${1}\" changed!"
#	    fi
#	    if (( ${3} == 1 )); then
#		grep -v "SHA256(${1})" "${2}" > "${2}.new"
#		echo ${NEW} >> "${2}.new"
#		mv "${2}.new" "${2}"
#	    fi
#	    L_RET=$XCCDF_RESULT_INFORMATIONAL
#	    ;;
#	"${E_NORECORD}")
#	    if (( ${4} == 1 )); then
#		report 'WARNING' $ID_CHKLIST_NORECORD "No SHA256 record for file \"${1}\""
#	    fi
#	    if (( ${3} == 1 )); then
#		echo ${NEW} >> "${2}"
#	    fi
#	    L_RET=$XCCDF_RESULT_INFORMATIONAL
#	    ;;
#	"${E_NOFILE}")
#	    if (( ${4} == 1 )); then
#		report 'WARNING' $ID_CHKLIST_FILENOTFOUND "File \"${1}\" not found."
#	    fi
#	    L_RET=$XCCDF_RESULT_INFORMATIONAL
#	    ;;
#	*)
#	    # FIXME: ...
#	    L_RET=$XCCDF_RESULT_FAIL
#	    ;;
#    esac
#    return $L_RET
#}

# __ssh_getconf sshd_config HostKey
function __ssh_getconf () {
    local val="$(sed -n -e "s/^[^#]*[\s]*${2}\s\(.*\)/\1/p" ${1})"

    if [[ -z "${val}" ]]; then
	eval "echo \$${2}"
    else
	echo "${val}"
    fi
}

# __ssh_getoptconf <cmdline> HostKey
function __ssh_getoptconf {
    case "${2}" in
	-*)
	    proc_getopt "${1}" "${2}"
	    ;;
	*)
	    proc_getopt "${1}" '-o' | sed -n "s/\s*${2}\s*=\(.*\)/\1/p"

	    case "${2}" in
		HostKey)
		    local OPT='-h'
		    ;;
		ServerKeyBits)
		    local OPT='-b'
		    ;;
		Port)
		    local OPT='-p'
		    ;;
		LoginGraceTime)
		    local OPT='-g'
		    ;;
		KeyRegenerationInterval)
		    local OPT='-k'
		    ;;
		*)
		    return ${E_FAIL}
		    ;;
	    esac
	    proc_getopt "${1}" ${OPT}
    esac
}

function __check_sshd_config () {
    local PROCDIR="$1"
    local CONFFILE="$2"
    local CONFDIR="$(dirname ${CONFFILE})"

    # check permissions dir + config

    check_file_perm "${CONFDIR}"  ${SSHD_CONFIGDIR_PERM} ${SSHD_CONFIGDIR_OWNER} 1 $ID_SSHD_CONFDIRPERM "sshd configuration directory"
    update_RET $?

    check_file_perm "${CONFFILE}" ${SSHD_CONFIG_PERM}    ${SSHD_CONFIG_OWNER}    1 $ID_SSHD_CONFDIRPERM "sshd configuration file"
    update_RET $?

    if (( ${SSH_KNOWN_HOSTS_REQUIRE} == 1 )); then
	check_file_perm "${CONFDIR}/ssh_known_hosts" ${SSH_KNOWN_HOSTS_PERM} ${SSH_KNOWN_HOSTS_OWNER} 1 $ID_SSHD_KNOWNDBPERM "ssh known hosts database"
	update_RET $?
    fi

    if (( ${SSHD_PRIVSEP_DIR_REQUIRE} == 1 )); then
	check_file_perm "${SSHD_PRIVSEP_DIR}" ${SSHD_PRIVSEP_DIR_PERM} ${SSHD_PRIVSEP_DIR_OWNER} 1 $ID_SSHD_PRIVSEPPERM "directory used by sshd during privilege separation in the pre-authentication phase"
	update_RET $?

	if (( ${SSHD_PRIVSEP_DIR_EMPTY} == 1 )); then
	    if [ -n "$(ls -A1 "${SSHD_PRIVSEP_DIR}")" ]; then
		report 'WARNING' $ID_PRIVSEPDIR_NOTEMPTY "Directory \"${SSHD_PRIVSEP_DIR}\" is not empty. This directory is used by sshd in the pre-authentication phase."
		report 'HINT'    $ID_PRIVSEPDIR_NOTEMPTY "Remove all unnecessary files from this directory."
		update_RET $XCCDF_RESULT_INFORMATIONAL
	    fi
	fi
    fi

    # protocol versions

    local PVER="$(__ssh_getconf "${CONFFILE}" Protocol)"

    if [[ -z "${PVER}" ]]; then
	local SSH_VER1=1
	local SSH_VER2=1
    else
	case "${PVER}" in
	    *1*)
		local SSH_VER1=1
		report 'WARNING' $ID_SSHD_DEPPROTOVER "SSHv1 is enabled. Using of this version is DEPRECATED."
		update_RET $XCCDF_RESULT_INFORMATIONAL
		;;
	    *)
		local SSH_VER1=0
	esac

	case "${PVER}" in
	    *2*)
		local SSH_VER2=1
		;;
	    *)
		local SSH_VER2=0
		;;
	esac
    fi

    # resolve keyfile names + permission check
    # keyfiles from config

    if [[ "${PROCDIR}" != "0" ]]; then
	while read keyfile; do
	    if [[ -n "${keyfile}" ]]; then
		case "${keyfile}" in
		    /*)
			keyfile="$(rel2abs "${keyfile}")"
			;;
		    *)
			keyfile="$(rel2abs "$(proc_getenv "${PROCDIR}/environ" PWD)/${keyfile}")"
			;;
		esac
		
		check_file_perm "${keyfile}"     ${SSHD_PRIVKEY_PERM} ${SSHD_PRIVKEY_OWNER}  1 $ID_SSHD_PRIVKEYPERM "sshd private key - from configuration file"
		update_RET $?

		check_file_perm "${keyfile}.pub" ${SSHD_PUBKEY_PERM}  ${SSHD_PUBKEY_OWNER}   1 $ID_SSHD_PUBKEYPERM  "sshd public key - from configuration file"
		update_RET $?

		#check_sha256 "${keyfile}"      "${TDATA_DIR}/${SSHD_SHA256_LIST}" "${SSHD_SAVE_SHA256_CHANGES}" ${SHOWSHAMSG}
		#check_sha256 "${keyfile}.pub"  "${TDATA_DIR}/${SSHD_SHA256_LIST}" "${SSHD_SAVE_SHA256_CHANGES}" ${SHOWSHAMSG}
	    fi
	done <<EOF
$((echo -e "${SSHD_DEFAULT_KEYS}"; __ssh_getconf "${CONFFILE}" HostKey) | sort | uniq)
EOF
    # keyfiles from -o,-h options
	while read keyfile; do
	    if [[ -n "${keyfile}" ]]; then
		case "${keyfile}" in
		    /*)
			keyfile="$(rel2abs "${keyfile}")"
			;;
		    *)
			keyfile="$(rel2abs "$(proc_getenv "${PROCDIR}/environ" PWD)/${keyfile}")"
			;;
		esac
		
		check_file_perm "${keyfile}"     ${SSHD_PRIVKEY_PERM} ${SSHD_PRIVKEY_OWNER} 1 $ID_SSHD_OPTPRIVKEYPERM "sshd private key - from options"
		update_RET $?

		check_file_perm "${keyfile}.pub" ${SSHD_PUBKEY_PERM}  ${SSHD_PUBKEY_OWNER}  1 $ID_SSHD_OPTPUBKEYPERM  "sshd public key - from options"
		update_RET $?

		#check_sha256 "${keyfile}"      "${TDATA_DIR}/${SSHD_SHA256_LIST}" "${SSHD_SAVE_SHA256_CHANGES}" ${SHOWSHAMSG}
		#check_sha256 "${keyfile}.pub"  "${TDATA_DIR}/${SSHD_SHA256_LIST}" "${SSHD_SAVE_SHA256_CHANGES}" ${SHOWSHAMSG}
	    fi
	done <<EOF
$(__ssh_getoptconf "${PROCDIR}/cmdline" HostKey)
EOF
    else
	while read keyfile; do
	    if [[ -n "${keyfile}" && -f "${keyfile}" ]]; then
		case "${keyfile}" in
		    /*)
			keyfile="$(rel2abs "${keyfile}")"
			;;
		    *)
			
			test_exit "${E_FAIL}" "Default configuration files MUST be specified using absolute path"
			#keyfile="$(rel2abs "$(dirname "${CONFFILE}")/${keyfile}")"
			;;
		esac
		
		check_file_perm "${keyfile}"     ${SSHD_PRIVKEY_PERM} ${SSHD_PRIVKEY_OWNER}  1 $ID_SSHD_PRIVKEYPERM "sshd private key - default configuration file"
		update_RET $?

		check_file_perm "${keyfile}.pub" ${SSHD_PUBKEY_PERM}  ${SSHD_PUBKEY_OWNER}   1 $ID_SSHD_PUBKEYPERM  "sshd public key - default configuration file"
		update_RET $?
	      
		#check_sha256 "${keyfile}"      "${TDATA_DIR}/${SSHD_SHA256_LIST}" "${SSHD_SAVE_SHA256_CHANGES}" ${SHOWSHAMSG}
		#check_sha256 "${keyfile}.pub"  "${TDATA_DIR}/${SSHD_SHA256_LIST}" "${SSHD_SAVE_SHA256_CHANGES}" ${SHOWSHAMSG}
	    fi
	done <<EOF
$(echo -e "${SSHD_DEFAULT_KEYS}" | sort | uniq)
EOF
    fi
    
    # options check

    case "$(__ssh_getconf "${CONFFILE}" PermitRootLogin)" in
	no)
	    ;;
	yes)
	    if (( ${PARANOID} == 1 )); then
		report 'ERROR' $ID_SSHD_REMOTEROOTTRUE "Remote root login IS ENABLED."
		update_RET $XCCDF_RESULT_FAIL
	    fi
	    ;;
	without-password)
	    report 'WARNING' $ID_SSHD_REMOTEROOTPUBKEY "Remote root login IS ENABLED only with publickey authentication."
	    update_RET $XCCDF_RESULT_INFORMATIONAL
	    ;;
	forced-commands-only)
	    report 'WARNING' $ID_SSHD_REMOTEROOTFORCEDONLY "Remote root login IS ENABLED, only forced commands allowed."
	    update_RET $XCCDF_RESULT_INFORMATIONAL
	    ;;
	*)
	    if (( ${PARANOID} == 1 )); then
	    	report 'WARNING' $ID_SSHD_REMOTEROOTNOTFALSE "Remote root login IS NOT EXPLICITLY DISABLED."
	    	report 'HINT'    $ID_SSHD_REMOTEROOTNOTFALSE "The default value is not known. If you don't want to allow remote root login, then you should disable it in your sshd_config."
		update_RET $XCCDF_RESULT_INFORMATIONAL
	    fi
	    ;;
    esac

    if [[ -z "$(__ssh_getconf "${CONFFILE}" AllowGroups)" ]]; then
	if [[ -z "$(__ssh_getconf "${CONFFILE}" AllowUsers)" && ${PARANOID} == 1 ]]; then
	    report 'WARNING' $ID_SSHD_ACCESSNOTRESTRICTED "Remote access IS NOT restricted with the AllowGroups nor the AllowUsers directive"
	    report 'HINT'    $ID_SSHD_ACCESSNOTRESTRICTED "It is recommended to resctrict remote login only to users that needs it or create a special group for this purpose."
	    update_RET $XCCDF_RESULT_INFORMATIONAL
	fi
    fi

    if [[ "$(__ssh_getconf "${CONFFILE}" ChallengeResponseAuthentication)" != "yes" && \
	"$(__ssh_getconf "${CONFFILE}" UsePAM)" != "yes" ]]; then
	report 'WARNING' $ID_SSHD_CHRESPAMDISABLED "Challenge-response and PAM authentication ARE DISABLED."
	update_RET $XCCDF_RESULT_INFORMATIONAL
    fi

    if [[ "$(__ssh_getconf "${CONFFILE}" PermitUserEnvironment)" != "no" ]]; then
	report 'WARNING' $ID_SSHD_USERENVENABLED "Processing of user's enviroment IS ENABLED."
	update_RET $XCCDF_RESULT_INFORMATIONAL
    fi

    if [[ "$(__ssh_getconf "${CONFFILE}" StrictModes)" != "yes" ]]; then
	report 'WARNING' $ID_SSHD_STRICTMODESDISABLED "StrictModes DISABLED."
	update_RET $XCCDF_RESULT_INFORMATIONAL
    fi

    if [[ "$(__ssh_getconf "${CONFFILE}" UsePrivilegeSeparation)" != "yes" ]]; then
	report 'WARNING' $ID_SSHD_PRIVSEPDISABLED "Privilege separation IS DISABLED."
	update_RET $XCCDF_RESULT_FAIL
    fi

    if (( ${SSH_VER2} == 1 && ${PARANOID} == 1 )); then
	# SSHv2
	local NUM=$(__ssh_getconf "${CONFFILE}" ClientAliveCountMax)

	if [[ $NUM ]]; then
	    if (( $NUM == 0 )); then
		report 'WARNING' $ID_SSHD_CLIALIVEMAXZERO "ClientAliveCountMax is set to 0 (no limit)"
		update_RET $XCCDF_RESULT_INFORMATIONAL
	    elif (( $NUM > 6 )); then
		report 'WARNING' $ID_SSHD_CLIALIVEMAXHIGH "ClientAliveCountMax is greater than 6"
		update_RET $XCCDF_RESULT_INFORMATIONAL
	    fi
	fi

	NUM=$(__ssh_getconf "${CONFFILE}" ClientAliveInterval)

	if [[ $NUM ]]; then
	    if (( $NUM == 0 )); then
		report 'WARNING' $ID_CLIALIVEINTZERO "ClientAliveInterval set to 0 (no limit)"
		update_RET $XCCDF_RESULT_INFORMATIONAL
	    elif (( $NUM > 600 )); then
		report 'WARNING' $ID_CLIALIVEINTLONG "ClientAliveInterval is higher than 600 seconds"
		update_RET $XCCDF_RESULT_INFORMATIONAL
	    fi
	fi
    fi
}

# 'check' functions for packages
# <name> <procdir>
# CHK_<pkg_name> ()
# e.g.: openssh-server-4.5p1-6.fc7 -> CHK_openssh_server ()

function CHK_openssh_server () {
    local PROCDIR="${2}"
    local PKGNAME="${1}"
    local DEFAULT_CONF="$(rpmquery -l "${PKGNAME}" | grep 'sshd_config$')"

    if [[ -z "${DEFAULT_CONF}" ]]; then
	test_exit ${E_FAIL} "Can't get the location of default configuration file for openssh-server."
    fi
    
    if [[ "${PROCDIR}" != "0" ]]; then
	local CURRENT_CONF="$(proc_getopt "${PROCDIR}/cmdline" -f)"
	local PUSER="$(uid2user $(proc_getuid "${PROCDIR}/status"))"
	local PGROUP="$(gid2user $(proc_getgid "${PROCDIR}/status"))"
    else
	local CURRENT_CONF=""
	local PUSER="${SSHD_USER}"
	local PGROUP="${SSHD_GROUP}"
    fi

    if [[ "${PUSER}"  != "${SSHD_USER}" ]]; then
	report 'WARNING' $ID_SSHD_WRONGPUSER "Wrong process owner (${PUSER}) - required is ${SSHD_USER}"
	update_RET $XCCDF_RESULT_FAIL
    fi

    if [[ "${PGROUP}" != "${SSHD_GROUP}" ]]; then
	report 'WARNING' $ID_SSHD_WRONGPGRP "Wrong process group (${PGROUP}) - required is ${SSHD_GROUP}"
	update_RET $XCCDF_RESULT_FAIL
    fi

    unset PUSER
    unset PGROUP

    if [[ -n "${CURRENT_CONF}" ]]; then
	case "${CURRENT_CONF}" in
	    /*)
		CURRENT_CONF="$(rel2abs "${CURRENT_CONF}")"
		;;
	    *)
		CURRENT_CONF="$(rel2abs "$(proc_getenv "${PROCDIR}/environ" PWD)/${CURRENT_CONF}")"
		;;
	esac

	if [[ "${CURRENT_CONF}" != "${DEFAULT_CONF}" ]] && notchecked "${CURRENT_CONF}"; then
	    # check current
	    __check_sshd_config "${PROCDIR}" "${CURRENT_CONF}"
	    addchecked "${CURRENT_CONF}"
	fi
    fi

    # check default
    if notchecked "${DEFAULT_CONF}"; then
	__check_sshd_config "${PROCDIR}" "${DEFAULT_CONF}"
	addchecked "${DEFAULT_CONF}"
    fi
}

function CHK_openssh_clients () {
    local PROCDIR="${2}"
    local PKGNAME="${1}"
    local PKGPROG="$(basename $(readlink "${2}/exe"))"
    local PROGUID="$(proc_getuid "${PROCDIR}/status")"
    local PROGGID="$(proc_getgid "${PROCDIR}/status")"
    local PROGUSER="$(uid2user "${PROGUID}")"
    local PROGGRP="$(gid2user "${PROGGID}")"

    case "${PKGPROG}" in
	ssh-agent)

	    ;;
	ssh)

	    ;;
	sftp)

	    ;;
	scp)

	    ;;
	ssh-add)

	    ;;
	ssh-keygen)

	    ;;
	ssh-keyscan)

	    ;;
	ssh-keysign)

	    ;;
	*)

	    ;;
    esac
}


function CHK_user_files () {
    while read line; do
	eval "$(sed -n "s|^\([^\`\$:;]\+\):[^\`\$:;]\+:\([0-9]\+\):\([0-9]\+\):[^\`\$:;]*:\([^\`\$:;]\+\):\([^\`\$:;]*\)$|PWD_USER=\"\1\" PWD_HOME=\"\4\"|p" <<<"${line}")"

	if canLogIn "${PWD_USER}"; then
	    if [[ -d "${PWD_HOME}/.ssh" ]]; then
		PREFIX="${PWD_HOME}/.ssh"

		if [[ -f "${PREFIX}/authorized_keys2" ]]; then
		    report 'WARNING' $ID_FILE_AUTHKEYS2 "User \"${PWD_USER}\" is using deprecated authorized_keys2 file."
		    update_RET $XCCDF_RESULT_FAIL
		    if [[ -f "${PREFIX}/authorized_keys" ]]; then
			report 'WARNING' $ID_FILE_TWOAUTHKEYS "User \"${PWD_USER}\" is using both authorized_keys and authorized_keys2."
			update_RET $XCCDF_RESULT_INFORMATIONAL
		    fi
		fi

		if [[ -f "${PREFIX}/known_hosts2" ]]; then
		    report 'WARNING' $ID_FILE_KNOWN2 "User \"${PWD_USER}\" is using deprecated known_hosts2 file."
		    update_RET $XCCDF_RESULT_FAIL
		    if [[ -f "${PREFIX}/known_hosts" ]]; then
                        report 'WARNING' $ID_FILE_TWOKNOWNHOSTS "User \"${PWD_USER}\" is using both known_hosts and known_hosts2."
			update_RET $XCCDF_RESULT_INFORMATIONAL
                    fi
		fi
	    fi
	fi
    done <<EOF
$(getent passwd)
EOF
}

########################################

check_env || exit ${E_FAIL}

#SHOWSHAMSG=1
#if [[ ! -f "${TDATA_DIR}/${SSHD_SHA256_LIST}" ]]; then
#	report 'WARNING' $ID_FIRSTRUN "This is a first run of the test. Some parts of audit are skipped."
#	touch "${TDATA_DIR}/${SSHD_SHA256_LIST}" || test_exit ${E_FAIL} "Can't create a file in my permanent data directory."
#	SHOWSHAMSG=0
#	SSHD_SAVE_SHA256_CHANGES=1
#fi

LoadDefaults "SSHD" "$(rpmquery openssh-server | sed -n 's|^openssh-server-\([0-9]\+\)\.\([0-9]\+[a-z][0-9]\)-.*$|\1\2|p')"

for PID in $(netstat -nltp | sed -n 's|^.*[[:space:]]\+\([0-9]\{1,5\}\)/sshd|\1|p' | sort | uniq); do
    report 'INFO' 0 "Checking openssh-server configuration (PID=${PID})"
    CHK_openssh_server "openssh-server" "${PROC}/${PID}/"
done

for PID in $(netstat -nltxp | sed -n 's|^.*[[:space:]]\+\([0-9]\{1,5\}\)/[_a-zA-Z-]*ssh[^d][_a-zA-Z-]*[[:space:]]*.*|\1|p' | sort | uniq); do
    report 'INFO' 0 "Checking openssh-client configuration (PID=${PID})"
    CHK_openssh_clients "openssh-clients" "${PROC}/${PID}/"
done

# Default
report 'INFO' 0 "Checking default openssh-server configuration"
CHK_openssh_server "openssh-server" "0"
CHK_user_files

exit $RET
