#!/bin/bash
# Copyright (C) 2008 Red Hat, Inc.
# Written by Dan Kopecek <dkopecek@redhat.com>
# Adapted for SCE by Martin Preisler <mpreisle@redhat.com>

XINETDCONF="/etc/xinetd.conf"
XINETDDIR="/etc/xinetd.d/"
SERVICE_EXECUTABLE_PERM="755"
SERVICE_EXECUTABLE_OWNER="root:"

RET=$XCCDF_RESULT_PASS

function get_services () {
    sed -ne 's|^[[:space:]]*service[[:space:]]*\([[:alnum:]]*\)[[:space:]]*.*$|\1|p' "${1}"
}

function get_service_vars () {
    sed -ne '/^[[:space:]]*service[[:space:]]*'"${1}"'[[:space:]]*/,/^[[:space:]]*service[[:space:]]*.*$/ {
	/^[[:space:]]*[a-zA-Z_]*[[:space:]]*[-+]\{0,1\}=/,/^[[:space:]]*[a-zA-Z_]*[[:space:]]*=/ s|^[[:space:]]*\([a-zA-Z_]*\)[[:space:]]*\([-+]\{0,1\}\)=[[:space:]]*\(.*\)$|local\ \1="\3"\
local\ \1_op="\2"|p
}' "${2}"

}

function get_default_vars () {
    sed -ne '/^[[:space:]]*defaults[[:space:]]*$/,/^[[:space:]]*}[[:space:]]*$/ {
	/^[[:space:]]*[a-zA-Z_]*[[:space:]]*[-+]\{0,1\}=/,/^[[:space:]]*[a-zA-Z_]*[[:space:]]*=/ s|^[[:space:]]*\([a-zA-Z_]*\)[[:space:]]*\([-+]\{0,1\}\)=[[:space:]]*\(.*\)$|local\ \1="\3"\
local\ \1_op="\2"|p
}' "${1}"

}

function common_checks () {
    eval "${3}"
    eval "${1}"

    if [[ -n "${server}" ]]; then
	case "${server}" in
	    /*)
		check_file_perm "${server}" ${SERVICE_EXECUTABLE_PERM} ${SERVICE_EXECUTABLE_OWNER} 1 $ID_SERVICE_PERM "${2} service executable"
		;;
	    *)
		echo "Executable path for service ${service} IS NOT ABSOLUTE."
		RET=$XCCDF_RESULT_FAIL
		;;
	esac
    fi

    return ${OK}
}

function check_auth () {
    eval "${2}"
    eval "${1}"

    if [[ "${disabled}" != "yes" && "${enabled}" != "no" ]] && [[ "${disable}" != "yes" ]]; then
	echo "Warning: auth service IS ENABLED"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
}

function check_ident () {
    eval "${2}"
    eval "${1}"

    if [[ "${disabled}" != "yes" && "${enabled}" != "no" ]] && [[ "${disable}" != "yes" ]]; then
	echo "Warning: ident service IS ENABLED"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
}

function check_systat () {
    eval "${2}"
    eval "${1}"

    if [[ "${disabled}" != "yes" && "${enabled}" != "no" ]] && [[ "${disable}" != "yes" ]]; then
	echo "Warning: systat service IS ENABLED"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
}

function check_chargen () {
    eval "${2}"
    eval "${1}"

    if [[ "${disabled}" != "yes" && "${enabled}" != "no" ]] && [[ "${disable}" != "yes" ]]; then
	echo "Warning: chargen service IS ENABLED"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
}

function check_echo () {
    eval "${2}"
    eval "${1}"

    if [[ "${disabled}" != "yes" && "${enabled}" != "no" ]] && [[ "${disable}" != "yes" ]]; then
	echo "Warning: echo service IS ENABLED"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
}

function check_finger () {
    eval "${2}"
    eval "${1}"

    if [[ "${disabled}" != "yes" && "${enabled}" != "no" ]] && [[ "${disable}" != "yes" ]]; then
	echo "Warning: finger service IS ENABLED"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
}

function check_netstat () {
    eval "${2}"
    eval "${1}"

    if [[ "${disabled}" != "yes" && "${enabled}" != "no" ]] && [[ "${disable}" != "yes" ]]; then
	echo "Warning: netstat service IS ENABLED"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
}

function check_rusers () {
    eval "${2}"
    eval "${1}"

    if [[ "${disabled}" != "yes" && "${enabled}" != "no" ]] && [[ "${disable}" != "yes" ]]; then
	echo "Warning: rusers service IS ENABLED"
	[ "$RET" == $XCCDF_RESULT_FAIL ] || RET=$XCCDF_RESULT_INFORMATIONAL
    fi
}

CONFIGFILES=""
DEFAULTS=""

if [[ -f "${XINETDCONF}" ]]; then
    CONFIGFILES="${XINETDCONF}"
    DEFAULTS="$(get_default_vars "${XINETDCONF}")"
fi

if [[ -d "${XINETDDIR}" ]]; then
    CONFIGFILES="$(find ${XINETDDIR} -type f) ${CONFIGFILES}"
fi

for CONF in ${CONFIGFILES}; do
    for service in $(get_services "${CONF}"); do
	VARS="$(get_service_vars "${service}" "${CONF}")"

	if [[ "$(type -t "check_${service}")" == "function" ]]; then
	    common_checks "${VARS}" "${service}" "${DEFAULTS}"
	    check_${service} "${VARS}" "${DEFAULTS}"
	fi
    done
done

exit $RET

