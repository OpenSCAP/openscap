#!/bin/bash

# Copyright 2014 Red Hat Inc., Durham, North Carolina.
# All Rights Reserved.

set -e -o pipefail

. $srcdir/../test_common.sh

test_init "test_bz2.log"

test_run "DataStream operations .xml.bz2" $srcdir/test_bz2_datastream.sh

test_exit
