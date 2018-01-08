#!/usr/bin/env bash

# Author:
#      Peter Vrabec, <pvrabec@redhat.com>
#	   Daniel Kopecek <dkopecek@redhat.com>


MITRE_FILES="/tmp/support"
EXTVARFILE="${MITRE_FILES}/var/linux-external-variables.xml"

DISTRO="$(cat /etc/*-release | head -n1)"
DISTRO_NAME="$(cat /etc/*-release | awk '{print $1}' | head -n1)"
DISTRO_RELEASE="$(cat /etc/*-release | sed -n 's|^[^0-9]*\([0-9]*\).*$|\1|p' | head -n1)"
SELINUX_ENABLED=$(grep -iqE "(SELINUX=enforcing|SELINUX=permissive)" \
	/etc/selinux/config 2>/dev/null; echo $?)

function test_mitre {

    require "egrep" || return 255
    require "awk" || return 255

    if [ ! -d "$MITRE_FILES" ]; then
    	tar zxf ${srcdir}/support.tgz -C /tmp
        # workaround file access time issue
        find "$MITRE_FILES"
	# workaround old schema version in linux-external-variables.xml
	sed -i 's/5.0/5.5/' "$EXTVARFILE"
    fi

    local ret_val=0;
    local DEFFILE=${srcdir}/$1
    local RESFILE=$1.results

    [ -f $RESFILE ] && rm -f $RESFILE
    $OSCAP oval eval --results "$RESFILE" --variables "$EXTVARFILE"  "$DEFFILE"
    # catch error from oscap tool
    ret_val=$?
    if [ $ret_val -eq 1 ]; then
	return 1
    fi

    if [ -n "$3" ]; then
        # only check the individual results specified in the arg list
        shift 1
        while [ -n "$1" -a -n "$2" ]; do
            def_id="$1"
            def_res="$2"
            grep -q "definition_id=\"$def_id\".*result=\"$def_res\"" "$RESFILE" || return 1
            shift 2
        done
        return 0
    fi

    # assume all definitions should have the same result

    LINES=`grep definition_id "$RESFILE"`
    # catch error from grep
    ret_val=$?
    if [ $ret_val -eq 2 ]; then
	return 1
    fi

    # calculate return code
    echo "$LINES" | grep -q -v "result=\"$2\""
    ret_val=$?
    if [ $ret_val -eq 1 ]; then
        return 0;
    elif [ $ret_val -eq 0 ]; then
	return 1;
    else
	return "$ret_val"
    fi
}

function cleanup_mitre {
    rm -rf "$MITRE_FILES"
}
