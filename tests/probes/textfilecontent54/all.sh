#!/bin/bash

. $builddir/tests/test_common.sh

test_init "test_probes_textfilecontent54.log"
test_run "textfilecontent54 general functionality" $srcdir/test_probes_textfilecontent54.sh
test_run "validate OVAL definitions of various schema versions" $srcdir/test_validation_of_various_oval_versions.sh
test_run "test behavior on symlinks" $srcdir/test_symlinks.sh
test_run "test multiline behavior" $srcdir/test_behavior_multiline.sh
test_exit
