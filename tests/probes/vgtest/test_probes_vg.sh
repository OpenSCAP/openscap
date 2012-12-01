#!/usr/bin/env bash

# Copyright 2009 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.
#
# OpenScap Probes Test Suite.
#
# Created on: Nov 30, 2009
#
# Authors:
#      Peter Vrabec, <pvrabec@redhat.com>
#      Ondrej Moris, <omoris@redhat.com>

. ${srcdir}/../../test_common.sh

# Test Cases.

VGTEST_TMPDIR="/tmp/vgtest"

function test_probes_vg {

    require "valgrind" || return 255
    
    if [[ ! -d "$VGTEST_TMPDIR" ]]; then
	mkdir -p "$VGTEST_TMPDIR" || exit 1
    fi
    
    EXCEPTIONS="( rpminfo runlevel_B interface )"

    for xmlgen in `find ${srcdir}/../ -name '*.xml.sh'`; do
	skipme=0
	for E in {${EXCEPTIONS[@]}; do
	    echo $xmlgen | grep $E && skipme=1 && break
	done	
	if [ $skipme -eq 0 ]; then
	    bash "$xmlgen" > "$VGTEST_TMPDIR/$(basename "$xmlgen" \
                  | sed 's|.sh$||')"
	    echo "Generating $(basename "$xmlgen" | sed 's|.sh$||')"
	else
	    echo "Skipping $(basename "$xmlgen" | sed 's|.sh$||')"
	fi
    done

    for xml in `find ${srcdir}/../../ -name '*.xml'`; do
	cp "$xml" "$VGTEST_TMPDIR"
	echo "Copying $xml"
    done

    export SEAP_DEBUGLOG_DISABLE=1
    export SEXP_VALIDATE_DISABLE=1
        
    echo ""
    echo "---------------- Valgrind checks -----------------"
    for xml in "$VGTEST_TMPDIR"/*.xml; do
	bash $srcdir/vgrun.sh "$OSCAP oval eval --results $xml.res $xml"
	[ $? -eq 0 ] || ret=1
    done
    echo "--------------------------------------------------"

    return 0
}

function cleanup {
    rm -rf "$VGTEST_TMPDIR"
}

# Testing.

test_init "test_probes_vg.log"

test_run "test_probes_vg" test_probes_vg

test_exit cleanup
