#!/bin/bash
#
# Copyright (C) 2012 Red Hat, Inc.
# Author(s): Daniel Kopecek <dkopecek@redhat.com>
#

CHECK_TCP=1
CHECK_UDP=1
CHECK_UNIX=1
CHECK_UPDATES=0
#CHECK_CONFIG=1
RECORD_DURATION=1
YUM_PIDFILE="/var/run/yum.pid"
# Options to test unconfined processes
# selinux type to match (can be regexp)
SETYPE_MATCH="unconfined_t\|initrc_t"
# range of ports to be checked for listening processes
PORT_MIN=0
PORT_MAX=1024
# message types for different report messages
DAEMONS_ROOT_REPORTTYPE="ERROR"
DAEMONS_USER_REPORTTYPE="NONE"
LISTENERS_ROOT_REPORTTYPE="ERROR"
LISTENERS_USER_REPORTTYPE="WARNING"

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
const ID_CANTFINDEXEPATH=1
const ID_CANTFINDPKGTOFILE=2
const ID_CHKLIST_FILECHANGED=3
const ID_CHKLIST_FILENOTFOUND=4
const ID_CHKLIST_NORECORD=5
const ID_CHKLIST_NOTFOUND=6
const ID_CKHLIST_NEW=7
const ID_UPDATESAVAIL=8

const ID_UNCONFINED_DAEMON=9
const ID_UNCONFINED_LISTENER=10

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

function report {
    echo "$1: $3"
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

function do_report {
	[ "x$1" != 'xNONE' ] && [ "x$1" != 'x' ] && report "$@"
}

# return values listed in all lists
function intersection {
	for i in `echo $*`; do echo $i; done  | \
	sort | uniq -c | grep "^\s*$# " | sed 's/^\s*[0-9]\+ //'
}

function lines_to_list {
	local DELIM=','
	[ $# == 1 ] && DELIM=$1
	echo $* | sed "s/ /$DELIM/g"
}

# get processes listening on priviledged ports
# output (one process per line): protocol port pid
function get_priv_listeners {
	netstat --numeric-ports --numeric-hosts -l -A inet -p \
		| awk '{ if (NF == 6) { print $1,$4,$6 } else { print $1,$4,$7 } }' \
		| tail -n+3 | sed -e 's/[0-9.]\+:\([0-9]\+\)/\1/' -e 's/\([0-9]\+\)\/.*$/\1/' \
		| grep '^[a-z]\+ [0-9]\+ [0-9]\+$' \
		| awk "{ if (\$2 >= $PORT_MIN && \$2 <= $PORT_MAX) { print \$0 } }"
}

# output daemon processes
function get_daemons {
	local TERMINALLESS=`ps -t- -o pid=`
	local INIT_CHILDREN=`ps --ppid=1 -o pid=`
	local SESSIONLEADERS=`ps --no-headers -e -o pid,sid | awk '{ if ($1==$2) { print $1 } }'`
	intersection "${TERMINALLESS}" "${INIT_CHILDREN}" "${SESSIONLEADERS}"
}

# get_unconfined processes
function get_unconfined {
	local CTX_PART="[a-zA-Z0-9_.-]\+"
	local PIDS=`lines_to_list $*`
	#echo $PIDS
	
	if [ "${PIDS}" == '' ]; then return 0; fi
	ps -p "${PIDS}" -o pid,label --no-headers  | \
		grep "^\s*[0-9]\+\s\+${CTX_PART}:${CTX_PART}:\\(${SETYPE_MATCH}\\):${CTX_PART}\$" | \
		awk '{ print $1 }'
}

# filter_selinux_type regexp_setype_t
# stdin: pids
# stdout: filtered pids
function filter_selinux_type {
	return 0
}

function daemons_report {
	local pid=
	for pid in $(get_unconfined `get_daemons`)
	do
		local uid=$(ps -p $pid -o uid --no-headers)
		local REPORTTYPE='NONE'
		if [ $uid == '0' ]; then
			REPORTTYPE="$DAEMONS_ROOT_REPORTTYPE"
		else
			REPORTTYPE="$DAEMONS_USER_REPORTTYPE"
		fi

		local user=$(ps -p $pid -o user --no-headers)
		local comm=$(ps -p $pid -o comm --no-headers)
		local cntx=$(ps -p $pid -o label --no-headers)
		do_report "$REPORTTYPE" "$ID_UNCONFINED_DAEMON" \
			"Unconfined ($cntx) daemon $comm with pid $pid running user $user detected"
		update_RET $XCCDF_RESULT_FAIL
		#do_report "HINT" "$ID_UNCONFINED_DAEMON" "Blah blah"
	done
}

function listeners_report {
	local listeners=`get_priv_listeners`
	local pids=`cut -d' ' -f3 <<<"$listeners"`
	local unconfined=`get_unconfined $pids`
	for pid in $unconfined
	do
		local uid=`ps -p $pid -o uid --no-headers`
		local REPORTTYPE='NONE'
		if [ $uid == '0' ]; then
			REPORTTYPE="$LISTENERS_ROOT_REPORTTYPE"
		else
                        REPORTTYPE="$LISTENERS_USER_REPORTTYPE"
                fi
		
		local user=`ps -p $pid -o user --no-headers`
		local comm=`ps -p $pid -o comm --no-headers`
		local cntx=`ps -p $pid -o label --no-headers`
		local entries=`echo "$listeners" | grep " $pid\$" | awk '{ print $1 "/" $2 }'`
		entries=`echo $entries` # get rid of newlines
		do_report "$REPORTTYPE" "$ID_UNCONFINED_LISTENER" \
			"Unconfined ($cntx) process $comm with pid $pid listening on port(s) $entries running user $user detected"
		#do_report "HINT" "$ID_UNCONFINED_LISTENER" "Blah blah"
		update_RET $XCCDF_RESULT_FAIL
	done
}


########################################

check_env || exit ${E_FAIL}

I=0
# TCP
if (( ${CHECK_TCP} == 1 )); then
    for p in `netstat -nlpt | sed -n 's|.*\s\([[:digit:]]\{1,5\}\)/[[:alnum:]]*.*|\1|p'`; do
	PID[$I]=${p}
	let I=$I+1
    done
fi

# UDP
if (( ${CHECK_UDP} == 1 )); then
    for p in `netstat -nlpu | sed -n 's|.*\s\([[:digit:]]\{1,5\}\)/[[:alnum:]]*.*|\1|p'`; do
	PID[$I]=${p}
	let I=$I+1
    done
fi

# SOCK
if (( ${CHECK_UNIX} == 1 )); then
    for p in `netstat -nlpx | sed -n 's|.*\s\([[:digit:]]\{1,5\}\)/[[:alnum:]]*.*|\1|p'`; do
	PID[$I]=${p}
	let I=$I+1
    done
fi

if (( $I == 0 )); then
    test_exit ${E_OK}
fi

# remove duplicates
TMP=${IFS}
IFS=$'\n'
PID=(`echo "${PID[*]}" | sort -n | uniq`)
IFS=${TMP}

# translate files into pkgnames
I=0
for p in ${PID[@]}; do 
   FILE="`readlink ${PROC}/${p}/exe 2> /dev/null`"

   case "${FILE}" in
   	*.#prelink#.*)
   	   FILE="`echo ${FILE} | sed -n 's|^\(.*\)\.#prelink#\..*$|\1|p'`"
	;;
   	*\(deleted\))
	   FILE="`echo ${FILE} | sed -n 's|^\(.*\)[[:space:]](deleted)$|\1|p'`"
	;;
   esac

   case "`basename \"${FILE}\"`" in
       python|python-*|php|php-*) # It's a plane! No, it's a mothership of universe peoples!! NO, IT'S P-Y-T-H-O-N (or PHP)!!!
	   WRKD="`proc_getenv \"${PROC}/${p}/environ\" PWD`"
	   FILE="`cat ${PROC}/${p}/cmdline | tr '\0' '\n' | grep -E '(^/)|(\./)' | head -n1`"
	   if [[ -n "${FILE}" ]]; then
	  	case "${FILE}" in
	  		/*)# absolute path
	  		;;
	  		*)
	  		   if [[ -n "${WRKD}" ]]; then
	  			FILE="${WRKD}/${FILE}"
	  		   else
	  		   	report 'WARNING' $ID_CANTFINDEXEPATH "Can't find out location of the script (PID: ${p})"
				update_RET $XCCDF_RESULT_FAIL
	  		   	continue
	  		   fi
	  		;;
	  	esac
	   else
		# FIXME
		continue
	   fi
	;;
   esac

   PKG="$(rpmquery -f --qf '%{NAME}' "${FILE}" 2> /dev/null)"

   if (( $? == 0 )); then
       PKGS[$I]=${PKG}
       INFO[$I]="${PKG} ${p}"
       let I=$I+1
   else
       report 'WARNING' $ID_CANTFINDPKGTOFILE "File \"${FILE}\" doesn't belong to any package."
       report 'HINT'    $ID_CANTFINDPKGTOFILE "The origin of this file is unknown."
       update_RET $XCCDF_RESULT_FAIL
   fi
done

# remove duplicates
TMP=${IFS}
IFS=$'\n'
PKGS=(`echo "${PKGS[*]}" | sort | uniq`)
INFO=(`echo "${INFO[*]}" | sort | uniq`)
IFS=${TMP}

if (( ${CHECK_UPDATES} != 0)); then
    YPID=$(cat "${YUM_PIDFILE}" 2> /dev/null)
    RUNP=$(ps -A --no-heading -o pid,comm | sed -n "s/^[[:space:]]*${YPID}[[:space:]]*yum\$/yes/p")

    if [[ -z "${YPID}" || -z "${RUNP}" ]]; then
    	MSG="$(yum -d 1 check-update ${PKGS[@]})"
    	
    	if (( $? == 100 )); then
		report 'WARNING' $ID_UPDATESAVAIL "Updates available: $(echo "${MSG}" | sed -n 's/\([[:alpha:]]*\)\s.*/\1/p' | tr '\n' ' ')"
		report 'HINT'    $ID_UPDATESAVAIL "Update your system to get rid of this message."
		update_RET $XCCDF_RESULT_INFORMATIONAL
    	fi
    	unset MSG
    fi
fi

unset PKGS

#for ((i = 0 ; i < ${#INFO[*]}; i++ )); do
#    PKG=$(echo ${INFO[$i]} | awk '{print $1}')
#    PID=$(echo ${INFO[$i]} | awk '{print $2}')
#
#    FNAME="`echo ${PKG} | tr QWERTYUIOPASDFGHJKLZXCVBNM- qwertyuiopasdfghjklzxcvbnm_`"
#
#    if (( ${CHECK_CONFIG} == 1 )); then
#	if [[ "`type -t CHK_${FNAME}`" == "function" ]]; then
#	    CHK_${FNAME} "${PKG}" "${PROC}/${PID}/"
#	#else
#	#    if [[ -d "${SECTOOL_SCRIPT_DIR}" ]]; then
#	#	for source in `find \"${SECTOOL_SCRIPT_DIR}\" -nowarn -name \'${PKG}.*\' -type f -perm 740 -user root -group root`; do
#	#	    . "${script}"
#	#        done
#	#    fi
#	fi
#    fi
#done

daemons_report
listeners_report

exit ${RET}
