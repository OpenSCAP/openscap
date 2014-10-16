#!/bin/bash

. ../../test_common.sh

test_init test_maskattr.log
test_run "object entity with mask" $srcdir/test_object_entity_mask.sh
test_run "object entity without mask" $srcdir/test_object_entity_nomask.sh
test_exit
