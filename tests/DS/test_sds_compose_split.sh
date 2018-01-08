#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

set -e -o pipefail

. $builddir/tests/test_common.sh

# Test Cases.

function assert_correct_xlinks()
{
	local DS=$1
	local stderr=$(mktemp)
	$OSCAP info $DS 2> $stderr
	diff $stderr /dev/null
	rm $stderr

	# First of all make sure that there is at least one ds:component-ref.
	[ "$($XPATH $DS 'count(//*[local-name()="component-ref"])')" != "0" ]
	# We want to catch cases when this element has different namespace.
	local ns=$($XPATH $DS 'name(//*[local-name()="component-ref"][1])' | sed 's/:.*$/:/')
	[ "$ns" != "component-ref" ] || ns=""
	# Ensure there is at least some xlink.
	[ "`$XPATH $DS \"count(//${ns}component-ref/@xlink:href)\"`" != "0" ]
	# This asserts that there is none component-ref/xlink:href broken.
	# Previously, we have seen datastreams with broken xlinks (see trac#286).
	[ "`$XPATH $DS  \"count(//${ns}component-ref[substring(@xlink:href, 2, 10000) != (//${ns}component/@${ns}id | //${ns}extended-component/@${ns}id)])\"`" == "0" ]
}

function test_sds {

    local DIR="${srcdir}/$1"
    local XCCDF_FILE="$2"
    local SKIP_DIFF="$3"
    local DS_TARGET_DIR="`mktemp -d`"
    local DS_FILE="$DS_TARGET_DIR/sds.xml"

    pushd "$DIR"

    $OSCAP ds sds-compose "$XCCDF_FILE" "$DS_FILE"

    assert_correct_xlinks $DS_FILE
    popd

    pushd "$DS_TARGET_DIR"

    $OSCAP ds sds-split "`basename $DS_FILE`" "$DS_TARGET_DIR"

    rm "$DS_FILE"

    # get rid of filler prefix to make the diff work
    for file in scap_org.open-scap_cref_*;
    do
        mv "$file" "${file#scap_org.open-scap_cref_}"
    done

    popd

    if [ "$SKIP_DIFF" != "1" ]; then
        if ! diff --exclude "oscap_debug.log.*" "$DIR" "$DS_TARGET_DIR"; then
            echo "The files are different after going through source data stream!"
            echo
            return 1
        fi
    fi

    rm -r "$DS_TARGET_DIR"
    return 0
}

# Testing.
test_init "test_sds_compose_split.log"

test_run "sds_simple" test_sds sds_simple scap-fedora14-xccdf.xml 0
test_run "sds_simple OVAL 5.11.1" test_sds sds_simple_5_11_1 simple_xccdf.xml 0
test_run "sds_multiple_oval" test_sds sds_multiple_oval multiple-oval-xccdf.xml 0
test_run "sds_missing_oval-prepare" [ ! -f sds_missing_oval/second-oval.xml ]
test_run "sds_missing_oval" test_sds sds_missing_oval multiple-oval-xccdf.xml 0
test_run "sds_subdir" test_sds sds_subdir subdir/scap-fedora14-xccdf.xml 1
test_run "sds_extended_component" test_sds sds_extended_component fake-check-xccdf.xml 0
test_run "sds_extended_component_plain_text" test_sds sds_extended_component_plain_text fake-check-xccdf.xml 0
test_run "sds_extended_component_plain_text_entities" test_sds sds_extended_component_plain_text_entities fake-check-xccdf.xml 0
test_run "sds_extended_component_plain_text_whitespace" test_sds sds_extended_component_plain_text_whitespace fake-check-xccdf.xml 0

test_exit
