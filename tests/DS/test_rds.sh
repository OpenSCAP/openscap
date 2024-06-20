#!/usr/bin/env bash

# Author:
#   Martin Preisler <mpreisle@redhat.com>

set -e -o pipefail

. $builddir/tests/test_common.sh
. $srcdir/test_ds_common.sh

# Test Cases.
function test_rds_index
{
    local ret_val=0;

    local RDS_FILE="${srcdir}/$1"
    local ASSETS="$2"
    local REPORTS="$3"
    local REQUESTS="$4"

    INDEX=$($OSCAP info "$RDS_FILE")

    for asset in "$ASSETS"; do
        if ! echo $INDEX | grep --quiet "$asset"; then
            ret_val=1
            echo "Asset $asset expected in index"
        fi
    done

    for report in "$REPORTS"; do
        if ! echo $INDEX | grep --quiet "$report"; then
            ret_val=1
            echo "Report $report expected in index"
        fi
    done

    for requests in "$REQUESTS"; do
        if ! echo $INDEX | grep --quiet "$request"; then
            ret_val=1
            echo "Report request $request expected in index"
        fi
    done

    return "$ret_val"
}


# Testing.
test_init

test_run "rds_index_simple" test_rds_index rds_index_simple/arf.xml "asset0 asset1" "report0" "collection0"

test_exit

