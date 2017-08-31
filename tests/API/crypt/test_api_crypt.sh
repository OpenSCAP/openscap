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
#      David Niemoller
#      Ondrej Moris, <omoris@redhat.com>

. ../../test_common.sh

# Test Cases.

function test_crapi_digest {
    local ret_val=0;
    local TEMPDIR="$(mktemp -d -t -q tmp.XXXXXX)"
    local sum_md5="";
    local sum_sha1="";
    local sum_sha256="";

    dd if=/dev/urandom of="${TEMPDIR}/a" count=1   bs=1k || return 2
    dd if=/dev/urandom of="${TEMPDIR}/b" count=123 bs=1  || return 2
    dd if=/dev/urandom of="${TEMPDIR}/c" count=1   bs=8k || return 2
    dd if=/dev/urandom of="${TEMPDIR}/d" count=321 bs=1  || return 2
    dd if=/dev/urandom of="${TEMPDIR}/e" count=1   bs=1024k || return 2
    dd if=/dev/urandom of="${TEMPDIR}/f" count=312 bs=1  || return 2
    
    for file in a b c d e f; do
        sum_md5=$((md5sum "${TEMPDIR}/${file}" || openssl md5 "${TEMPDIR}/${file}") | sed -n 's|^.*\([0-9a-f]\{32\}\).*$|\1|p')
        sum_sha1=$((sha1sum "${TEMPDIR}/${file}" || openssl sha1 "${TEMPDIR}/${file}") | sed -n 's|^.*\([0-9a-f]\{40\}\).*$|\1|p')
        sum_sha256=$((sha256sum "${TEMPDIR}/${file}" || openssl sha256 "${TEMPDIR}/${file}") | sed -n 's|^.*\([0-9a-f]\{64\}\).*$|\1|p')

        if [[ "$sum_md5" == "" || "$sum_sha1" == "" || "$sum_sha256" == "" ]]; then
            return 2
        fi

        ./test_crapi_digest "${TEMPDIR}/${file}" "$sum_md5" "$sum_sha1" "$sum_sha256" || return 1
        #echo "$file: ret $?, 5: $sum_md5, 1: $sum_sha1, $sum_sha256"
    done

    rm -rf "$TEMPDIR"

    return 0
}

function test_crapi_mdigest {
    local ret_val=0;
    local TEMPDIR="$(mktemp -d -t -q tmp.XXXXXX)"
    local sum_md5="";
    local sum_sha1="";
    local sum_sha256="";

    dd if=/dev/urandom of="${TEMPDIR}/a" count=1   bs=1k || return 2
    dd if=/dev/urandom of="${TEMPDIR}/b" count=123 bs=1  || return 2
    dd if=/dev/urandom of="${TEMPDIR}/c" count=1   bs=8k || return 2
    dd if=/dev/urandom of="${TEMPDIR}/d" count=321 bs=1  || return 2
    dd if=/dev/urandom of="${TEMPDIR}/e" count=1   bs=1024k || return 2
    dd if=/dev/urandom of="${TEMPDIR}/f" count=312 bs=1  || return 2
    
    for file in a b c d e f; do
        sum_md5=$((md5sum "${TEMPDIR}/${file}" || openssl md5 "${TEMPDIR}/${file}") | sed -n 's|^.*\([0-9a-f]\{32\}\).*$|\1|p')
        sum_sha1=$((sha1sum "${TEMPDIR}/${file}" || openssl sha1 "${TEMPDIR}/${file}") | sed -n 's|^.*\([0-9a-f]\{40\}\).*$|\1|p')
        sum_sha256=$((sha256sum "${TEMPDIR}/${file}" || openssl sha256 "${TEMPDIR}/${file}") | sed -n 's|^.*\([0-9a-f]\{64\}\).*$|\1|p')

        if [[ "$sum_md5" == "" || "$sum_sha1" == "" || "$sum_sha256" == "" ]]; then
            return 2
        fi

        ./test_crapi_mdigest "${TEMPDIR}/${file}" "$sum_md5" "$sum_sha1" "$sum_sha256" || return 1
        #echo "$file: ret $?, 5: $sum_md5, 1: $sum_sha1"
    done

    rm -rf "$TEMPDIR"

    return 0
}

# Testing.

test_init "test_api_crypt.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "test_crapi_digest" test_crapi_digest
    test_run "test_crapi_mdigest" test_crapi_mdigest
fi

test_exit
