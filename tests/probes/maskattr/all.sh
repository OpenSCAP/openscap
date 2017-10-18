#!/bin/bash

. $builddir/tests/test_common.sh

test_init test_maskattr.log
test_run "object entity with mask - OVAL 5.10" $srcdir/test_object_entity_mask.sh
test_run "object entity with mask - OVAL 5.9" $srcdir/test_object_entity_mask_oval_5_9.sh
test_run "object entity without mask" $srcdir/test_object_entity_nomask.sh
test_exit
