#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

. ../test_common.sh

# Test Cases.

function test_no_external_imports {
    # openscap XSD schemas shall NOT import other XSDs from external
    # locations!

    # We have to do this pathetic double grep because grep doesn't support
    # multiline matching. Normally pcregrep could be used but since it's not
    # installed very often we use this hack instead. It assumes that xsd:import
    # declarations don't take more than 2 lines in total.
    EXTERNAL_IMPORTS=$(grep -n -P "schemaLocation=\"http[s]?://[^\"]+\"" -C 1 -r "${srcdir}/../../schemas/" | grep -P "<[^:]+:import")

    if [ "$?x" != "1x" ]; then
        echo "XSD schemas using external xsd:import found!:"
        echo
        echo "$EXTERNAL_IMPORTS"
        echo

        return 1
    fi

    return 0
}

# Testing.
test_init "test_schemas.log"

if [ -z ${CUSTOM_OSCAP+x} ] ; then
    test_run "no_external_imports" test_no_external_imports
fi

test_exit
