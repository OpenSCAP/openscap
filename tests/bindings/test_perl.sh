#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

. ../test_common.sh

# Test Cases.

function test_perl_import {

    #echo "require openscap" | perl
    #return $?
    # FIXME: We currently skip the test, this should be uncommented when
    #        perl bindings are working again
    return 255
}

# Testing.
test_init "test_perl.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "perl_import" test_perl_import
fi

test_exit
