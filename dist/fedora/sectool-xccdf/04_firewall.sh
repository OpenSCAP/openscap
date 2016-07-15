#!/bin/bash

# -------------------------------- #
# Copyright (C) 2008 Red Hat, Inc.
# Written by Dan Kopecek <dkopecek@redhat.com>
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>
# Thu Jan 31 14:53:38 CET 2008
# firewall.sh
# TODO: - learn extended regexp and fix stupid regexps in sed
#       - IPv6
#	- fix generating of internal IPs (full arp table)
# -------------------------------- #

RET=$XCCDF_RESULT_PASS

E_OK=0
E_FAIL=1

ENABLE_IPV4=1
ENABLE_IPV4_FWCHECK=1
SCAN_TCP4=0
SCAN_UDP4=0
ENABLE_IPV6=1
ENABLE_IPV6_FWCHECK=0
SCAN_TCP6=0
SCAN_UDP6=0
DEV_BLACKLIST="virbr[0-9]* lo"
PARANOID=0

# --- Constants ------------------ #

IP4_REGEX='\([[:digit:]]\{1,3\}\.[[:digit:]]\{1,3\}\.[[:digit:]]\{1,3\}\.[[:digit:]]\{1,3\}\)\(/\([[:digit:]]\{1,2\}\)\)'
IP6_REGEX=''
PROCDIR="`mount | sed -n 's/.*on\s\(.*\)\stype proc.*/\1/p'`"

MYPID=$$
MONPID=0
#const MONOUT="`mktemp ${TEMP_DIR}/${NAME}.mon.XXXXXX`"
ORIG_TTL=64
TTLWATCHDOGPID=0

# --- Error IDs ------- #
ID_FWACCEPTPOLICY=1
ID_FWACCEPTPOLICY6=2
ID_FWDROPPOLICY=3
ID_FWDROPPOLICY6=4
ID_FWNOREJECT=5
ID_FWNOREJECT6=6
ID_FWNOTENABLED=7
ID_FWNOTENABLED6=8
ID_FWQUEUEPOLICY=9
ID_FWQUEUEPOLICY6=10
ID_IP4_ALL_SRCROUTE_ENABLED=11
ID_IP4_DEF_SRCROUTE_ENABLED=12
ID_IP4_DEV_SRCROUTE_ENABLED=13
ID_IP4_FORWARDING_ENABLED=14
ID_IP6_FORWARDING_ENABLED=15
ID_OPENPORTTCP=16
ID_OPENPORTUDP=17

# --- Network related commands --- #

C_ROUTE="/sbin/route"
C_NETSTAT="/bin/netstat"
C_IP="/sbin/ip"
C_LSOF="/usr/sbin/lsof"
C_NMAP="/usr/bin/nmap"
C_PING="/bin/ping"
C_PING6="/bin/ping6"
C_ARP="/sbin/arp"
C_TCPDUMP="/usr/sbin/tcpdump"
C_SYSCTL="/sbin/sysctl"
C_IPTABLES="/sbin/iptables"
C_IP6TABLES="/sbin/ip6tables"

# --- Internal ------------------- #

if [[ -z "${PROCDIR}" ]]; then
	echo "Can't access proc directory (${PROCDIR})"
	exit $XCCDF_RESULT_ERROR
fi

# --- Functions ------------------ #

# <tcpdump filter>
function start_mon () {
    ${C_TCPDUMP} -o ${MONOUT} ${1} &
    MONPID=$!
}

# void
function stop_mon () {
    kill ${MONPID}
    cat  ${MONOUT}
    echo -n > ${MONOUT}
}

# <new ttl>
function set_system_ttl () {
	if [[ -z "${1}" ]]; then
		echo "Internal error: set_system_ttl(): missing argument"
		exit $XCCDF_RESULT_ERROR
	fi

	if (( ${1} > 0 && ${1} < 256 )); then
		if [[ -f "${PROCDIR}/sys/net/ipv4/ip_default_ttl" ]]; then
			ORIG_TTL=$(cat "${PROCDIR}/sys/net/ipv4/ip_default_ttl")

			# TTL watchdog
			bash -c 'while `ps -A | grep -q ^[[:space:]]\*${MYPID}`; do sleep 1; done; echo ${ORIG_TTL} > /proc/sys/net/ipv4/ip_default_ttl || sysctl -w net.ipv4.ip_default_ttl=${ORIG_TTL};'
			TTLWATCHDOGPID=$!

			echo ${1} > "${PROCDIR}/sys/net/ipv4/ip_default_ttl" && return ${E_OK} || return ${E_FAIL}
		else	
			ORIG_TTL=$(sysctl -n net.ipv4.ip_default_ttl || test_exit ${E_FAIL} "Can't get default TTL for IPv4")
			sysctl -q -w net.ipv4.ip_default_ttl=${1} && return ${E_OK} || return ${E_FAIL}
		fi
	else
		return ${E_FAIL}
	fi

	return ${E_OK}
}

function reset_system_ttl () {
	if [[ -z "${ORIG_TTL}" ]]; then
		echo "Internal error: reset_system_ttl(): ORIG_TTL is empty!"
		exit $XCCDF_RESULT_ERROR
	fi

	if (( ${ORIG_TTL} > 0 && ${ORIG_TTL} < 256 )); then
		if [[ -f "${PROCDIR}/sys/net/ipv4/ip_default_ttl" ]]; then
			echo ${ORIG_TTL} > "${PROCDIR}/sys/net/ipv4/ip_default_ttl" && return ${E_OK} || return ${E_FAIL}
		else	
			sysctl -q -w net.ipv4.ip_default_ttl=${ORIG_TTL} && return ${E_OK} || return ${E_FAIL}
		fi
	else
		echo "Internal error: reset_system_ttl(): ORIG_TTL=\"${ORIG_TTL}\""
		exit $XCCDF_RESULT_ERROR
	fi

	# kill watchdog
	kill -9 ${TTLWATCHDOGPID}
	TTLWATCHDOGPID=0

	return ${E_OK}
}

# <bits><base>
function random () {
    case "${1}" in
	16d)
	    od -t u2 -N 2 -A n /dev/urandom | tr -d ' '
	    ;;
	16x)
	    od -t x2 -N 2 -A n /dev/urandom | tr -d ' '
	    ;;
	8d)
	    od -t u1 -N 1 -A n /dev/urandom | tr -d ' '
	    ;;
	8x)
	    od -t x1 -N 1 -A n /dev/urandom | tr -d ' '
	    ;;
	*)
	    echo 0
    esac
}

# <af>
function get_devices () {
    case "${1}" in
	4|inet)
	    local AF_OPT='-4'
	    ;;
	6|inet6)
	    local AF_OPT='-6'
	    ;;
	*)
	    local AF_OPT='-4'
	    ;;
    esac

   #DEBUGMSG "BLACKLIST: ${DEV_BLACKLIST}"
   #DEBUGMSG "($(echo "${DEV_BLACKLIST}" | sed -e 's,[[:space:]]\+,|,g' -e 's,\(^|\)\|\(|$\),,g'))"

    ${C_IP} ${AF_OPT} addr show | sed -n '/^[[:digit:]]*:[[:space:]]*/,/^[[:digit:]]*:[[:space:]]*/ s|^[[:digit:]]*:[[:space:]]*\([a-zA-Z]*[[:digit:]]*\):.*|\1|p' |\
	{
	  if [[ -n "${DEV_BLACKLIST}" ]]; then
		grep -vE "($(echo "${DEV_BLACKLIST}" | sed -e 's,[[:space:]]\+,|,g' -e 's,\(^|\)\|\(|$\),,g'))"
	  else
		cat
	  fi
	}
}

# <dev> <af>
function get_ips () {
    local DEV="${1}"
    local AF="${2}"

    case "${AF}" in
	4|inet)
	    local AF_OPT='-4'
	    local AF_KEYWORD='inet'
	    ;;
	6|inet6)
	    local AF_OPT='-6'
	    local AF_KEYWORD='inet6'
	    ;;
	*)
	    # Unknown address family
	    return ${E_FAIL}
    esac

    ${C_IP} ${AF_OPT} addr show | sed -n '/^[[:digit:]]*:[[:space:]]*'"${DEV}"':[[:space:]]*/,/^[[:digit:]]*:[[:space:]]*/ s/^[[:space:]]*'"${AF_KEYWORD}"'[[:space:]]*\([^[:space:]]*\).*/\1/p'
}

# void
function random_mac () {
    echo "$(random 8x):$(random 8x):$(random 8x):$(random 8x):$(random 8x):$(random 8x)"
}

# <host> <mac> [<dev>]
function arp_register_host () {

    if [[ -n "${3}" ]]; then
	local DEV_OPT="-i ${3}"
    fi

    #DEBUGMSG "Adding ${1}@${2} to arp table..."

    ${C_ARP} -n ${ARP_OPT} -s "${1}" "${2}" && return ${E_OK} || return ${E_FAIL}
}

# <host>
function arp_unregister_host () {

    #DEBUGMSG "Deleting ${1} from arp table..."
    ${C_ARP} -n -d "${1}" && return ${E_OK} || return ${E_FAIL}
}

DEFAULT_PORT=$(random 16d)
RANDOM_MAC=$(random_mac)

# <source dev> {tcp|udp} {4|6} <from ip> <to ip> <to port> <ttl>
function check_port () {
    local DEV="${1}"
    local PROTO="${2}"
    local AF="${3}"
    local FROM="${4}"
    local TO_ADDR="${5}"
    local TO_PORT="${6}"
    local TTL="${7}"

    case "${AF}" in
	4)
	    case "${PROTO}" in
		[Tt][Cc][Pp])
		    ${C_NMAP} --send-ip --spoof-mac ${RANDOM_MAC} -sS -n --ttl ${TTL} -T5 -P0 -e ${DEV} -S ${FROM} -p ${TO_PORT} ${TO_ADDR}
		    ;;
		[Uu][Dd][Pp])
		    ${C_NMAP} --send-ip --spoof-mac ${RANDOM_MAC} -sU -n --ttl ${TTL} -T5 -P0 -e ${DEV} -S ${FROM} -p ${TO_PORT} ${TO_ADDR}
		    ;;
		*)
		    # Unknown protocol
		    return ${E_FAIL}
	    esac
	    ;;
	6)
	    case "${PROTO}" in
		[Tt][Cc][Pp])
		    ${C_NMAP} --send-ip --spoof-mac ${RANDOM_MAC} -6 -n --ttl ${TTL} -sS -T5 -P0 -e ${DEV} -S ${FROM} -p ${TO_PORT} ${TO_ADDR}
		    ;;
		[Uu][Dd][Pp])
		    ${C_NMAP} --send-ip --spoof-mac ${RANDOM_MAC} -6 -n --ttl ${TTL} -sS -T5 -P0 -e ${DEV} -S ${FROM} -p ${TO_PORT} ${TO_ADDR}
		    ;;
		*)
		    # Unknown protocol
		    return ${E_FAIL}
	    esac
	    ;;
	*)
	    # Unknown address family
	    return ${E_FAIL}
    esac
}

# <source dev> {tcp|udp|icmp} {4|6} <from ip> <to ip> <ttl>
function send_packet () {
    local DEV="${1}"
    local PROTO="${2}"
    local AF="${3}"
    local FROM="${4}"
    local TO="${5}"
    local TTL="${6}"

    case "${AF}" in
	4)
	    case "${PROTO}" in
		[Tt][Cc][Pp])
		    ${C_NMAP} --send-ip --spoof-mac ${RANDOM_MAC} --ttl ${TTL} -n -T5 -P0 -sS -p ${DEFAULT_PORT} -e ${DEV} -S ${FROM} ${TO} 2>&1 > /dev/null
		    ;;
		[Uu][Dd][Pp])
		    ${C_NMAP} --send-ip --spoof-mac ${RANDOM_MAC} --ttl ${TTL} -n -T5 -P0 -sU -p ${DEFAULT_PORT} -e ${DEV} -S ${FROM} ${TO} 2>&1 > /dev/null
		    ;;
		[Ii][Cc][Mm][Pp])
		    ${C_PING} -I ${DEV} ${FROM} ${TO} 2&1 > /dev/null
		    ;;
		*)
		    # Unknown protocol
		    return ${E_FAIL}
	    esac
	    ;;
	6)	    
	    case "${PROTO}" in
		[Tt][Cc][Pp])
		    ${C_NMAP} --send-ip --spoof-mac ${RANDOM_MAC} --ttl ${TTL} -n -T5 -P0 -6 -sS -p ${DEFAULT_PORT} -e ${DEV} -S ${FROM} ${TO} 2>&1 > /dev/null
		    ;;
		[Uu][Dd][Pp])
		    ${C_NMAP} --send-ip --spoof-mac ${RANDOM_MAC} --ttl ${TTL} -n -T5 -P0 -6 -sU -p ${DEFAULT_PORT} -e ${DEV} -S ${FROM} ${TO} 2>&1 > /dev/null
		    ;;
		[Ii][Cc][Mm][Pp])
		    ${C_PING6} -I ${DEV} ${FROM} ${TO} 2&1 > /dev/null
		    ;;
		*)
		    # Unknown protocol
		    return ${E_FAIL}
	    esac
	    ;;
	*)
	    # Unknown address family
	    return ${E_FAIL}
	    ;;
    esac

    # wtf?
    return ${E_FAIL}
}

function is_public_IPv4 () {
    local a=0
    local b=0
    local c=0
    local d=0

    read a b c d <<EOF
`echo ${1} | tr '.' ' '`
EOF
    if   (( ${a} == 192 && ${b} == 168 )); then
	return ${E_FAIL}
    elif (( ${a} == 10 )); then
	return ${E_FAIL}
    elif (( ${a} == 172 && ${b} >= 16 && ${b} <= 31 )); then
	return ${E_FAIL}
    elif (( ${a} == 169 && ${b} == 254 )); then
	return ${E_FAIL}
    elif (( ${a} == 127 )); then
	return ${E_FAIL}
    else
	return ${E_OK}
    fi
}

# <proto> <ip> [4|6]
function get_open_ports () {
    local IP=${2}
    local PROTO=${1}
    local AF=${3}

    case "${PROTO}" in
	[Tt][Cc][Pp])
	    local P_OPT='-t'
	    local P_KEY='tcp'
	    ;;
	[Uu][Dd][Pp])
	    local P_OPT='-u'
	    local P_KEY='udp'
	    ;;
	*)
	    # wtf?
	    return ${E_FAIL}
    esac

    for ip in `${C_NETSTAT} ${P_OPT} -ln | sed -n 's/^'"${P_KEY}"'[[:space:]]*[[:digit:]]*[[:space:]]*[[:digit:]]*[[:space:]]*\([^[:space:]]*\)[[:space:]]*.*/\1/p'`; do
	echo ${ip} | sed -n 's/^\(.*\):\([[:digit:]]*\)$/\1 \2/p' | while read addr port; do
	    case "${addr}" in
		${IP})
		echo ${port}
		;;
		0.0.0.0)
		if (( ${AF} == 4 )); then
		    echo ${port}
		fi
		;;
		::)
		# FIXME: IPv6 only
		echo ${port}
		;;
	    esac
	done
    done
}

function gen_rand_ip () {
	local IPARR=(`echo ${1} | tr '.' ' '`)

	for ((i=$2, a=3; i >= 8 && a >= 0; i-=8, --a)); do
		IPARR[$a]=$((($(random 8d) % 253) + 1))
	done

	#DEBUGMSG "a=${a}, i=${i}"
	#DEBUGMSG "IP=${1}, m=${2}"

	if ((i > 0)); then
		IPARR[$a]=$((${IPARR[$a]} | (($(random 8d) % ((2**i) - 1)) + 1)))
	fi

	echo ${IPARR[0]}.${IPARR[1]}.${IPARR[2]}.${IPARR[3]}
}

# FIXME...
function get_out_ip () {
	echo 192.168.0.1    
}

# <ip> <mask>
# address from IP/MASK network
function get_int_ip () {
	local BITS
	local CIDR
	local GIP
	local NET
	typeset -i BITS
	typeset -i CIDR

	if ! `echo ${2} | grep -q '^[0-9]\{1,2\}$'`; then
		CIDR=$(ipcalc -p ${1} ${2} | sed -n 's/^PREFIX=\([0-9]\{1,2\}\)$/\1/p')
	else
		CIDR=${2}
	fi

	NET="$(ipcalc -n ${1}/${CIDR} | sed -n 's|^NETWORK=\([[:digit:]]\{1,3\}\.[[:digit:]]\{1,3\}\.[[:digit:]]\{1,3\}\.[[:digit:]]\{1,3\}\)$|\1|p')"
	GIP="$(gen_rand_ip "${NET}" $((32 - $CIDR)))"

	while `arp -an | grep -q "${GIP}"`; do
		#DEBUGMSG "IP ${GIP} already exists in arp table, generating new one"
		GIP="$(gen_rand_ip "${NET}" $((32 - $CIDR)))"
	done

	echo ${GIP}
}

# FIXME...
function get_pub_ip () {
	echo 212.20.96.34
}

# <proto> <dev> <dev_ip> <dev_ipmask> <af> <from_type> <port_range> <ttl>
function scan_ports () {
    local PROTO="${1}"
    local DEV="${2}"
    local IP="${3}"
    local MASK="${4}"
    local AF="${5}"
    local FROM="${6}"
    local PORT_RANGE="${7}"
    local TTL="${8}"
    local ARPREG=0

    case "${PROTO}" in
	[Tt][Cc][Pp])
	    local NMAP_OPT='-sS'
	    ;;
	[Uu][Dd][Pp])
	    local NMAP_OPT='-sU'
	    ;;
	*)
	    # Unknown protocol
	    return ${E_FAIL}
    esac

    case "${AF}" in
	4|inet)
	    ;;
	6|inet6)
	    NMAP_OPT="-6 ${NMAP_OPT}"
	    ;;
	*)
	    # Unknown address family
	    return ${E_FAIL}
    esac

    case "${FROM}" in
	OUT) # private range, but out of (IP/MASK) network
	    FROM_IP="`get_out_ip ${IP} ${MASK}`"
	    ;;
	INT) # address from IP/MASK network
	    NMAP_OPT="${NMAP_OPT} --spoof-mac ${RANDOM_MAC}"
	    FROM_IP="`get_int_ip ${IP} ${MASK}`"
	    arp_register_host "${FROM_IP}" "${RANDOM_MAC}" "${DEV}" && ARPREG=1
	    ;;
	PUB) # random public address
	    FROM_IP="`get_pub_ip ${IP} ${MASK}`"
	    ;;
	SELF)
	    FROM_IP="${IP}"
	    ;;
	*)
	    echo "Internal error in scan_ports(): FROM=\"${FROM}\""
	    exit $XCCDF_RESULT_ERROR
    esac

    #DEBUGMSG "CMD: ${C_NMAP} ${NMAP_OPT} --send-ip --open -n --ttl ${TTL} -T5 -P0 -e ${DEV} -S ${FROM_IP} -p ${PORT_RANGE} ${IP}"

    set_system_ttl 1
    ${C_NMAP} ${NMAP_OPT} --send-ip --open -n --ttl ${TTL} -T5 -P0 -e ${DEV} -S ${FROM_IP} -p ${PORT_RANGE} -oG - ${IP} | sed -n -e 's|^.*Ports:[[:space:]]*\(.*\)$|\1|' -e 's|\([[:digit:]]\{1,5\}\)/[-a-z_]*/[-a-z_]*//[-a-z_]*///[,]\{0,1\}|\1|gp'
    reset_system_ttl

    if (( $ARPREG == 1 )); then
	arp_unregister_host ${FROM_IP}
    fi

    return ${E_FAIL}
}

# <chain> <severity> <err_ok>
function check_chain () {
    local CHAIN="$(${C_IPTABLES} -L | sed -n '/^Chain\ '"${1}"'/,/^$/ p')"
    local POLICY="$(echo -e "${CHAIN}" | sed -n 's/^Chain.*(policy\ \([A-Z]*\))$/\1/p')"
    local RULES="$(echo -e  "${CHAIN}" | sed -n '3,/^$/ s/[[:alnum:]]\{1,\}/&/p')"
    local SEVERITY="${2}"
    local ERROK=${3}

    #DEBUGMSG "CHAIN=${1}, POLICY=\"${POLICY}\" RULES=\"${RULES}\""

    case "${POLICY}" in
	ACCEPT)
	    if [[ -z "${RULES}" ]]; then
		echo "No firewall rules in IPv4 ${1} chain and policy is set to ACCEPT."
		RET=$XCCDF_RESULT_FAIL

		if (( $ERROK == 0 )); then
		    return ${E_FAIL}
		fi
	    else
		local LAST="$(echo -e "${RULES}" | sed -n '$ s/^\([[:alnum:]_-]*\)[[:space:]]*all[[:space:]]*.*$/\1/p')"

		if [[ -n "${LAST}" ]]; then
		    case "${LAST}" in
			REJECT|DROP)
			    return ${E_OK}
			    ;;
			ACCEPT)
			    echo "Policy for IPv4 ${1} chain is set to ACCEPT and the chain doesn't contain REJECT rule which should be the last rule in chains with this policy."
			    RET=$XCCDF_RESULT_FAIL
			    ;;
			*) # Custom chain
			    if ! check_chain "${LAST}" "${SEVERITY}" $ERROK; then
				echo "Custom IPv4 \"${1}\" chain is the last rule in ${1} chain with policy ACCEPT and doesn't contain REJECT rule which should be the last rule in chains with this policy."
				RET=$XCCDF_RESULT_FAIL
			    fi
			    ;;
		    esac
		else
		    echo "Policy for IPv4 ${1} chain is set to ACCEPT and the chain doesn't contain REJECT rule which should be the last in chains with this policy."
		    RET=$XCCDF_RESULT_FAIL
		fi
	    fi
	    ;;
	DROP)
	    if [[ -z "${RULES}" ]]; then
		echo "Policy for IPv4 ${1} chain is set to DROP without any rules."
		RET=$XCCDF_RESULT_FAIL

		if (( $ERROK == 0 )); then
		    return ${E_FAIL}
		fi
	    fi
	    ;;
	RETURN)
	    if [[ -z "${RULES}" ]]; then
		echo "Policy for IPv4 ${1} chain is set to RETURN without any rules."
		RET=$XCCDF_RESULT_FAIL		

		if (( $ERROK == 0 )); then
		    return ${E_FAIL}
		fi
	    fi
	    ;;
	QUEUE)
	    if [[ -z "${RULES}" ]]; then
		echo "Policy for IPv4 ${1} chain is set to QUEUE without any rules."
	    fi
	    ;;
	*)
	    case "${1}" in
		INPUT|OUTPUT|FORWARD)
		    echo "Internal error in check_chain(): POLICY=\"${POLICY}\", CHAIN=\"${1}\""
		    exit $XCCDF_RESULT_ERROR
		    ;;
		*) # Custom chain -> search for REJECT
		    if [[ -z "$(echo -e "${RULES}" | grep REJECT)" ]]; then
			# FIXME: more recursion..?
			return ${E_FAIL}
		    else
			return ${E_OK}
		    fi
		    ;;
	    esac
    esac

    return ${E_OK}
}

# <chain> <severity> <err_ok>
function check_chain6 () {
    local CHAIN="$(${C_IP6TABLES} -L | sed -n '/^Chain\ '"${1}"'/,/^$/ p')"
    local POLICY="$(echo -e "${CHAIN}" | sed -n 's/^Chain.*(policy\ \([A-Z]*\))$/\1/p')"
    local RULES="$(echo -e  "${CHAIN}" | sed -n '3,/^$/ s/[[:alnum:]]\{1,\}/&/p')"
    local SEVERITY="${2}"
    local ERROK=${3}

    #DEBUGMSG "CHAIN=${1}, POLICY=\"${POLICY}\" RULES=\"${RULES}\""

    case "${POLICY}" in
	ACCEPT)
	    if [[ -z "${RULES}" ]]; then
		echo "No firewall rules in IPv6 ${1} chain and policy is set to ACCEPT."
		RET=$XCCDF_RESULT_FAIL

		if (( $ERROK == 0 )); then
		    return ${E_FAIL}
		fi
	    else
		local LAST="$(echo -e "${RULES}" | sed -n '$ s/^\([[:alnum:]_-]*\)[[:space:]]*all[[:space:]]*.*$/\1/p')"

		if [[ -n "${LAST}" ]]; then
		    case "${LAST}" in
			REJECT)
			    return ${E_OK}
			    ;;
			ACCEPT)
			    echo "Policy for IPv6 ${1} chain is set to ACCEPT and the chain doesn't contain REJECT rule which should be the last rule in chains with this policy."
			    RET=$XCCDF_RESULT_FAIL
			    ;;
			*) # Custom chain
			    if ! check_chain "${LAST}" "${SEVERITY}" $ERROK; then
				echo "Custom IPv6 \"${1}\" chain is the last rule in ${1} chain with policy ACCEPT and doesn't contain REJECT rule which should be the last rule in chains with this policy."
				RET=$XCCDF_RESULT_FAIL
			    fi
			    ;;
		    esac
		else
		    echo "Policy for IPv6 ${1} chain is set to ACCEPT and the chain doesn't contain REJECT rule which should be the last in chains with this policy."
		    RET=$XCCDF_RESULT_FAIL
		fi
	    fi
	    ;;
	DROP)
	    if [[ -z "${RULES}" ]]; then
		echo "Policy for IPv6 ${1} chain is set to DROP without any rules."
		RET=$XCCDF_RESULT_FAIL

		if (( $ERROK == 0 )); then
		    return ${E_FAIL}
		fi
	    fi
	    ;;
	RETURN)
	    if [[ -z "${RULES}" ]]; then
		echo "Policy for IPv6 ${1} chain is set to RETURN without any rules."
		RET=$XCCDF_RESULT_FAIL

		if (( $ERROK == 0 )); then
		    return ${E_FAIL}
		fi
	    fi
	    ;;
	QUEUE)
	    if [[ -z "${RULES}" ]]; then
		echo "Policy for IPv6 ${1} chain is set to QUEUE without any rules."
	    fi
	    ;;
	*)
	    case "${1}" in
		INPUT|OUTPUT|FORWARD)
		    echo "Internal error in check_chain6(): POLICY=\"${POLICY}\", CHAIN=\"${1}\""
		    exit $XCCDF_RESULT_ERROR
		    ;;
		*) # Custom chain -> search for REJECT
		    if [[ -z "$(echo -e "${RULES}" | grep REJECT)" ]]; then
			# FIXME: more recursion..?
			return ${E_FAIL}
		    else
			return ${E_OK}
		    fi
		    ;;
	    esac
    esac

    return ${E_OK}
}

# --- Main ----------------------- #

if (( ${ENABLE_IPV4} == 1 )); then
    check_chain 'INPUT'   'ERROR'   0  || exit $RET

    if (( ${PARANOID} == 1 )); then
	check_chain 'OUTPUT'  'WARNING' 1  || exit $RET
    fi

    DEV4="$(get_devices 4)"
    FORWARDING4=0

    for device in ${DEV4}; do
	if [[ -f "${PROC}/sys/net/ipv4/conf/${device}/forwarding" ]]; then
	    if (( $(cat "${PROC}/sys/net/ipv4/conf/${device}/forwarding") == 1 )); then
		FORWARDING4=1
	    fi
	else
	    if (( $(${C_SYSCTL} -n net.ipv4.conf.${device}.forwarding) == 1 )); then
		FORWARDING4=1
	    fi
	fi
    done

    if (( $FORWARDING4 == 1 )); then
	echo "IPv4 forwarding is ENABLED."
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL

	check_chain 'FORWARD' 'WARNING' 1  || test_exit ${E_OK}
    fi

    SYSCTL_RET=$(${C_SYSCTL} -n net.ipv4.conf.all.accept_source_route)

    if (( $SYSCTL_RET != 0 )); then
	echo "IPv4 source routing is ENABLED on ALL interfaces!"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi

    SYSCTL_RET=$(${C_SYSCTL} -n net.ipv4.conf.default.accept_source_route)

    if (( $SYSCTL_RET != 0 )); then
	echo "IPv4 Source routing is ENABLED as default!"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi

    #DEBUGMSG "IPv4 devices: ${DEV4}"

    for device in ${DEV4}; do

	SYSCTL_RET=$(${C_SYSCTL} -n net.ipv4.conf.${device}.accept_source_route)

	if (( $SYSCTL_RET != 0 )); then
	    echo "IPv4 source routing is ENABLED on device \"${device}\""
	    [ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
	fi

	if (( $ENABLE_IPV4_FWCHECK == 1 )); then
	    #DEBUGMSG "-- Searching for IPv4 addresses on ${device}"
	    for device_ipm in `get_ips ${device} 4`; do
		#DEBUGMSG "---- Found: ${device_ipm}@${device}"

		read device_ip device_ip_mask <<EOF
`echo "${device_ipm}" | tr '/' ' '`
EOF

		#DEBUGMSG "ip: ${device_ip}"
		#DEBUGMSG "mask: ${device_ip_mask}"

		if (( ${SCAN_TCP4} == 1 )); then
			TCP_PORTS=(`get_open_ports tcp ${device_ip} 4 | sort -n`)
			echo "Scanning all TCP ports (65535) on ${device_ip}, this can take a long time..."
			S_TCP_PORTS=(`scan_ports tcp ${device} ${device_ip} ${device_ip_mask} 4 INT 1-65535 64 | sort -n`)

			if (( ${#TCP_PORTS[*]} > ${#S_TCP_PORTS[*]} )); then
			    echo "Internal error: TCP_PORTS > S_TCP_PORTS"
			    exit $XCCDF_RESULT_ERROR
			fi

			TMP=${IFS}
			IFS=$'\n'
			DIFF=(`echo -e "${TCP_PORTS[*]}\n${S_TCP_PORTS[*]}" | sort | uniq -u`)
			DIFF=(`echo -e "${DIFF1[*]}\n${S_TCP_PORTS[*]}" | sort | uniq -d`)
			IFS=${TMP}

			for ((a=0; a < ${#DIFF[*]}; ++a)); do
			    echo "TCP Port ${DIFF[$a]} is open on ${device_ip} but not visible in netstat."
			    [ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
			done
		fi

		if (( ${SCAN_UDP4} == 1 )); then
			UDP_PORTS=(`get_open_ports udp ${device_ip} 4 | sort -n`)
			echo "Scanning all UDP ports (65535) on ${device_ip}, this can take a long time..."
			S_UDP_PORTS=(`scan_ports udp ${device} ${device_ip} ${device_ip_mask} 4 INT 1-65535 64 | sort -n`)

			if (( ${#UDP_PORTS[*]} > ${#S_UDP_PORTS[*]} )); then
			    echo "Internal error: UDP_PORTS > S_UDP_PORTS"
			    exit $XCCDF_RESULT_ERROR
			fi

			TMP=${IFS}
			IFS=$'\n'
			DIFF=(`echo -e "${UDP_PORTS[*]}\n${S_UDP_PORTS[*]}" | sort | uniq -u`)
			DIFF=(`echo -e "${DIFF1[*]}\n${S_TCP_PORTS[*]}" | sort | uniq -d`)
			IFS=${TMP}

			for ((a=0; a < ${#DIFF[*]}; ++a)); do
			    echo "UDP Port ${DIFF[$a]} is open on ${device_ip} but not visible in netstat."
			    [ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
			done
		fi

		#if is_public_IPv4 ${device_ip}; then
		## -- Public IP -- #
		#    #DEBUGMSG "---- Public IPv4"
		#    
		## check private ranges <-> public ip
		#else
		## -- Intranet? -- #
		#    #DEBUGMSG "---- Private IPv4"
		#fi	    
	    done
	fi

    done
fi

if (( ${ENABLE_IPV6} == 1 )); then
    ${C_IP} -6 addr show | grep -q inet6
    if (( $? == 0 )); then
	check_chain6 'INPUT'   'ERROR'   0  || exit $RET

	if (( ${PARANOID} == 1 )); then
	    check_chain6 'OUTPUT'  'WARNING' 1  || exit $RET
	fi

	DEV6="$(get_devices 6)"
	FORWARDING6=0

	for device in ${DEV6}; do
	    if [[ -f "${PROC}/sys/net/ipv6/conf/${device}/forwarding" ]]; then
		if (( $(cat "${PROC}/sys/net/ipv6/conf/${device}/forwarding") == 1 )); then
		    FORWARDING6=1
		fi
	    else
		if (( $(${C_SYSCTL} -n net.ipv6.conf.${device}.forwarding) == 1 )); then
		    FORWARDING6=1
		fi
	    fi
	done

	if (( $FORWARDING6 == 1 )); then
	    echo "IPv6 forwarding is ENABLED."
	    [ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
	    check_chain6 'FORWARD' 'WARNING' 1  || exit $RET
	fi

    else
	echo "IPv6 checking is enabled but IPv6 is not supported on this system."
    fi
fi

# get devices
# get ips + open ports
# compare with netstat, lsof
# check source routing
# check private ranges 

exit $RET
# --- EOF ------------------------ #

