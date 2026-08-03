#!/usr/bin/env bash

# Copyright 2011 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

. $builddir/tests/test_common.sh

# Test Cases.

function test_probes_fileextendedattribute {
    probecheck "fileextendedattribute" || return 255

    local ret_val=0;
    local DEFFILE="$srcdir/test_probes_fileextendedattribute.xml"
    local RESFILE="$(mktemp results.XXXXXXX.xml)"

    [ -f $RESFILE ] && rm -f $RESFILE

    touch /tmp/xattr_with_val
    touch /tmp/xattr_without_val

    case $(uname) in
	FreeBSD)
		setextattr user fooattr foo /tmp/xattr_with_val
		setextattr user fooattr "" /tmp/xattr_without_val
		;;
	*)
		setfattr -n user.fooattr -v foo /tmp/xattr_with_val
		setfattr -n user.fooattr /tmp/xattr_without_val
		;;
    esac

    touch /tmp/xattr_noattr

    $OSCAP oval eval --results $RESFILE $DEFFILE

    if [ -f $RESFILE ]; then
	verify_results "def" $DF $RF 1 && verify_results "tst" $DF $RF 3
	ret_val=$?
    else
        ret_val=1
    fi

    return $ret_val
}

# Testing.
test_init
test_run "test_probes_fileattribute" test_probes_fileextendedattribute
test_exit
